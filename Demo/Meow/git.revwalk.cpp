
#include "app.precompiled.h"
#include "git.revwalk.h"

#include <Nena\PlatformAssist.h>
#include <git2.h>

#define _DiffSeparator "#---------------------------------------------------------------------"

namespace Nena
{
	namespace Git
	{
		template <typename _Ty1, typename _Ty2> 
		static bool Equals( _Ty1 *a, _Ty2 *b )
		{
			return (void *) a == (void *) b;
		}
		template <typename _Ty1, typename _Ty2>
		static bool Checked(_Ty1 flags, _Ty2 flag)
		{
			return (flags & flag) == flag;
		}
	}
}

Nena::Git::Revwalk::Commit::Commit(Nena::Git::Revwalk *host)
	: handle(NULL)
	, treeHandle(NULL)
	, parentsExpected(0)
	, host(host)
{
	Nena::Platform::ZeroValue(&Time);
}

Nena::Git::Revwalk::Commit::~Commit()
{
	::git_tree_free((::git_tree *) treeHandle);
	::git_commit_free((::git_commit *) handle);
}

typedef struct Nena::Git::Revwalk::Implementation : 
	protected Nena::Git::Revwalk::Commit::List
{
	friend Nena::Git::Revwalk;
	//! allow to iterate and modify items, but not modify a collection

	using Nena::Git::Revwalk::Commit::List::crbegin;
	using Nena::Git::Revwalk::Commit::List::rbegin;
	using Nena::Git::Revwalk::Commit::List::cbegin;
	using Nena::Git::Revwalk::Commit::List::begin;
	using Nena::Git::Revwalk::Commit::List::crend;
	using Nena::Git::Revwalk::Commit::List::rend;
	using Nena::Git::Revwalk::Commit::List::cend;
	using Nena::Git::Revwalk::Commit::List::end;

	typedef char PathBuffer[GIT_PATH_MAX];

	enum OnFailedOption
	{
		kNone = 0,
		kAssert = 1 << 0, // (yet unused)
		kDebugTrace = 1 << 1,
		kConsoleTrace = 1 << 2,
	};

	::git_repository *repositoryHandle;
	::git_revwalk *handle;

	Commit::List commitList;
	Revwalk *host;

	Implementation(Revwalk *publicRevwalk) 
		: host(publicRevwalk)
		, repositoryHandle(NULL)
		, handle(NULL)
	{
	}

	~Implementation()
	{
		//! free heap
		for (auto &i : *this) delete i;

		//! free git handles
		::git_revwalk_free(handle);
		::git_repository_free(repositoryHandle);
	}

	void Clear()
	{
		if (!empty()) 
		for (auto &i : *this) 
			delete i;
		clear();
	}

	concurrency::task<void> RunAsync(String verifiedRepository)
	{
		return concurrency::create_task([this, verifiedRepository]() { return Run(verifiedRepository); });
	}

	void Run(String const &verifiedRepository)
	{
		typedef std::pair<git_commit *, git_commit *> Commit2Commit;
		typedef std::multimap<git_commit *, git_commit *> Commit2Commits;

		Clear();
		if (repositoryHandle) ::git_repository_free(repositoryHandle);

		if (FailedTrace(
			::git_repository_open(&repositoryHandle, verifiedRepository.c_str()),
			_Meow_ItemToString(Revwalk::Implementation::Run), kConsoleTrace
			))
		{
			StatusUpdatedEventArgs args;
			args.Status = "Failed to open repository";
			host->StatusUpdated(host, &args);
		}

		// reuse previously created handle
		if (handle) ::git_revwalk_free(handle);
		//if (handle) ::git_revwalk_reset(handle);
		// or create a new rev walker handle
		//else 
		if (FailedTrace(::git_revwalk_new(&handle, repositoryHandle),
			_Meow_ItemToString(Revwalk::Implementation::Run), kConsoleTrace
			))
		{
			StatusUpdatedEventArgs args;
			args.Status = "Failed to create handle";
			host->StatusUpdated(host, &args);
			return;
		}

		if (FailedTrace(
			//! push the entire rev range
			//! @todo set this range externally, to switch branches
			::git_revwalk_push_range(handle, "origin/master..HEAD"),
			_Meow_ItemToString(Revwalk::Implementation::Run), kConsoleTrace
			))
		{
			StatusUpdatedEventArgs args;
			args.Status = "Failed to push handle range";
			host->StatusUpdated(host, &args);
			return;
		}

		::git_revwalk_sorting(handle, GIT_SORT_TIME | GIT_SORT_TOPOLOGICAL);

		int error;
		git_oid oid; 
		Commit2Commits parents;
		Nena::Platform::ZeroValue(&oid);

		while (!::git_revwalk_next(&oid, handle))
		{
			auto commitItem = new Commit(host);

			::git_commit *parent = 0;
			::git_commit *&commit = (::git_commit *&) commitItem->handle;
			::git_tree *&tree = (::git_tree *&) commitItem->treeHandle;

			if (FailedTrace(::git_commit_lookup(&commit, repositoryHandle, &oid)))
			{
				StatusUpdatedEventArgs args;
				args.Status = "Commit lookup failed";
				host->StatusUpdated(host, &args);

				delete commitItem;
				break;
			}
			else
			{
				commitItem->Id = ::git_oid_allocfmt(::git_commit_id(commit));
				commitItem->Message = ::git_commit_message(commit);

				auto author = ::git_commit_author(commit);
				_ASSERT_EXPR(author, L"git_commit_author");

				UnixTimeToSystemTime(author->when.time, commitItem->Time);
				commitItem->time = author->when.time;

				tm time;
				localtime_s(&time, &author->when.time);
				char timeBuffer[128] = { 0 };
				strftime(timeBuffer, sizeof timeBuffer, "%#c", &time);
				commitItem->LongTime = timeBuffer;
				strftime(timeBuffer, sizeof timeBuffer, "%x %X", &time);
				commitItem->ShortTime = timeBuffer;

				commitItem->AuthorEmail = author->email;
				commitItem->AuthorName = author->name;

				::git_commit_tree(&tree, commit);
				Commit::List::push_back(commitItem);

				auto parentCount = ::git_commit_parentcount(commit);
				commitItem->parentsExpected = parentCount;
				for (uint32_t parentIndex = 0; parentIndex < parentCount; parentIndex++)
				{ 
					error = ::git_commit_parent(&parent, commit, parentIndex);
					_ASSERT_EXPR(!error, L"git_commit_parent");

					parents.insert(Commit2Commit(commit, parent));
				}
			}
		}

		//! loop though all commits found
		for (auto &commitItem : *this)
		{
			auto parentPairs = parents.equal_range(
				(::git_commit *) commitItem->handle
				);

			//! loop though all the parents found for current commit
			for (auto i = parentPairs.first; i != parentPairs.second; i++)
			//! loop though all commits found again and find parent commits
			for (auto &j : *this) if (Equals(i->second, j->handle))
				//! push them to list back so that we could generate diffs later on
				commitItem->Parents.push_back(j);
			
			_ASSERT_EXPR( //! when debugging, check if its ok (just in case)
				commitItem->Parents.size() == commitItem->parentsExpected,
				L"Parents` count mismatch detected"
				);
		}

		parents.clear();
	}

#pragma region Error checking

	/**!
	 * Checks whether an error occured
	 * @returns git_error on error, null otherwise
	 */
	static git_error const *Check(int e)
	{
		if (e < 0) return ::giterr_last();
		else return NULL;
	}

	/**!
	 * Checks whether an error occured, and makes debug output
	 * @returns zero in succeeded, nonzero otherwise
	 */
	static BOOL FailedTrace(
		int code,
		const char *where = "revwalk",
		OnFailedOption o = OnFailedOption::kDebugTrace
		)
	{
		using namespace Nena::Platform;
		if (auto err = Check(code))
		{
			if (o & OnFailedOption::kDebugTrace) DebugTrace(
				"git> %s: %d %s\n", where, err->klass, err->message
				);
			else if (o & OnFailedOption::kConsoleTrace) std::cout << DebugTrace<FALSE>(
				"git> %s: %d %s\n", where, err->klass, err->message
				);
			return code;
		} else return ERROR_SUCCESS;
	}

#pragma endregion

#pragma region Utility

	/**!
	* Converts unix time to Windows system time
	* @see http://support.microsoft.com/kb/167296
	*/
	static void UnixTimeToSystemTime(
		git_time_t t, SYSTEMTIME &st
		)
	{
		int64_t ll; FILETIME ftl;
		ll = Int32x32To64(t, 10000000) + 116444736000000000LL;
		ftl.dwLowDateTime = (DWORD) ll;
		ftl.dwHighDateTime = ll >> 32;
		FileTimeToSystemTime(&ftl, &st);
	}

	/**!
	* Encodes string to URL-compatible one
	* @returns URL string
	*/
	static String ToUrl(String const &value)
	{
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << hex;

		for (String::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
		{
			String::value_type c = (*i);
			if (::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') escaped << c;
			else if (c == ' ') escaped << '+';
			else escaped << '%' << std::setw(2) << ((int) c) << std::setw(0);
		}

		return escaped.str();
	}

	/**!
	* Replaces one occurence of @p from by @p to in @p str 
	* @returns @c TRUE if replacement occured, @c FALSE otherwise 
	* @see http://stackoverflow.com/a/3418285
	*/
	static bool Replace(String &str, String const &from, String const &to)
	{
		size_t start_pos = str.find(from);
		if (start_pos == String::npos) return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	/**!
	* Replaces all occurances of @p from by @p to in @p str 
	* @returns number of occurences replaced
	* @see http://stackoverflow.com/a/3418285
	*/
	static UINT32 ReplaceAll(String &str, String const &from, String const &to)
	{
		if (from.empty()) return 0;

		size_t start_pos = 0;
		UINT32 replacements = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
			str.replace(start_pos, from.length(), to),
			start_pos += to.length(),
			replacements;
		return replacements;
	}

#pragma endregion

	typedef struct _ForEachFile_GitDiffCallback_Payload
	{
		std::stringstream *ss = NULL;
	} _foreach_diffcb_data;

	static int _foreach_diffcb(
		const git_diff_delta *delta,
		float progress,
		void *payload
		)
	{
		auto p = (_foreach_diffcb_data *) payload;
		if (!p || !p->ss) return GIT_ERROR; // break

		auto &ss = (*p->ss);

		ss << "<b>";

		switch (delta->status)
		{
		case ::GIT_DELTA_ADDED: ss << "(add) "; break;
		case ::GIT_DELTA_DELETED: ss << "(del) "; break;
		case ::GIT_DELTA_RENAMED: ss << "(ren) "; break;
		case ::GIT_DELTA_MODIFIED: ss << "(mod) "; break;
		}

		ss << "</b><i>";

		switch (delta->status)
		{
		case ::GIT_DELTA_ADDED:
		case ::GIT_DELTA_DELETED:
		case ::GIT_DELTA_MODIFIED:
			ss << delta->old_file.path;
			break;
		case ::GIT_DELTA_RENAMED:
			ss << delta->old_file.path << " => " 
			   << delta->new_file.path;
			break;
		}

		ss << "</i><br>";
		return GIT_OK;
	}

} _revwalk_private;


Nena::Git::Revwalk::Revwalk()
	: impl(new Implementation(this))
{

}

Nena::Git::Revwalk::~Revwalk()
{
	delete impl;
}

BOOL Nena::Git::Revwalk::DiscoverRepository(
	String const &startLocation,
	String &gitLocation
	)
{
	Implementation::PathBuffer gitLocationBuffer = { 0 };

	if (Implementation::FailedTrace(
		::git_repository_discover(
			gitLocationBuffer, ARRAYSIZE(gitLocationBuffer),
			startLocation.c_str(), FALSE, NULL),
		_Meow_ItemToString(Revwalk::DiscoverRepository),
		Implementation::kConsoleTrace))
			return FALSE;

	gitLocation = gitLocationBuffer;
	return TRUE;
}

Nena::Git::Revwalk::ActionAsync Nena::Git::Revwalk::RunAsync(String r)
{
	return impl->RunAsync(r);
}

void Nena::Git::Revwalk::Run(String const &r)
{
	return impl->Run(r);


}

Nena::Git::Revwalk::Commit::List::size_type 
Nena::Git::Revwalk::Size() const
{
	return impl->size();
}

Nena::Git::Revwalk::String 
Nena::Git::Revwalk::Commit::GenerateDiffPatch(
	UINT32 parent,
	BOOL markup
	) const
{
	if (parent == UINT_MAX) 
		return GenerateDiffPatch(
			Parents, markup
			);
	else if (parent < Parents.size()) 
		return GenerateDiffPatch(
			**::std::next(Parents.begin(), parent), markup
			);

	return String();
}

Nena::Git::Revwalk::String 
Nena::Git::Revwalk::Commit::GenerateDiffPatch(
Commit const &commit,
BOOL markup
) const
{
	int error = GIT_OK;
	::CHAR *buff = NULL;
	::git_diff *diff = NULL;
	::git_patch *patch = NULL;

	::git_tree // revs` trees
		*oldTree = NULL, // parent commit
		*newTree = NULL; // this commit
	if (time > commit.time)
		newTree = (::git_tree *) treeHandle,
		oldTree = (::git_tree *) commit.treeHandle;
	else
		oldTree = (::git_tree *) treeHandle,
		newTree = (::git_tree *) commit.treeHandle;

	::git_diff_options diffopts;
	::Nena::Platform::ZeroValue(&diffopts);
	::git_diff_options_init(&diffopts, GIT_DIFF_OPTIONS_VERSION);
	diffopts.flags |= GIT_DIFF_MINIMAL;
	diffopts.flags |= GIT_DIFF_PATIENCE;
	/*diffopts.flags |= GIT_DIFF_SHOW_UNMODIFIED;
	diffopts.flags |= GIT_DIFF_IGNORE_FILEMODE;
	diffopts.flags |= GIT_DIFF_INCLUDE_UNTRACKED;
	diffopts.flags |= GIT_DIFF_INCLUDE_TYPECHANGE;
	diffopts.flags |= GIT_DIFF_INCLUDE_TYPECHANGE_TREES;*/

	if (Implementation::FailedTrace(
		::git_diff_tree_to_tree(&diff, host->impl->repositoryHandle, oldTree, newTree, &diffopts),
		_Meow_ItemToString(Revwalk::Commit::GenerateDiffPatch),
		Implementation::kConsoleTrace))
		return ("error: failed to generate diff");

	_revwalk_private::_foreach_diffcb_data d;
	::std::stringstream ss; d.ss = &ss;
	error = ::git_diff_foreach(diff, _revwalk_private::_foreach_diffcb,
		NULL, NULL, (Revwalk::Unknown) &d
		);

	if (Implementation::FailedTrace(
		::git_patch_from_diff(&patch, diff, 0),
		_Meow_ItemToString(Revwalk::Commit::GenerateDiffPatch),
		Implementation::kConsoleTrace))
	{
		::git_diff_free(diff);
		return ("error: failed to generate diff patch");
	}
	if (Implementation::FailedTrace(
		::git_patch_to_str(&buff, patch),
		_Meow_ItemToString(Revwalk::Commit::GenerateDiffPatch),
		Implementation::kConsoleTrace))
	{
		::git_patch_free(patch), ::git_diff_free(diff);
		return ("error: failed to get patch text");
	}

	ss << "<br>" << buff;
	return ss.str();
}

Nena::Git::Revwalk::String 
Nena::Git::Revwalk::Commit::GenerateDiffPatch(
	List const &commits,
	BOOL markup
	) const
{
	std::stringstream ss;
	if (!markup)
	{
		ss << _DiffSeparator << "\n";
		ss << "commit " << Id << ":\n";
		ss << _DiffSeparator << "\n";
		for (auto &i : commits)
			ss << ">> commit " << (*i).Id << ":\n" <<
			(*i).GenerateDiffPatch(*this, markup) << "\n" <<
			_DiffSeparator << "\n\n";
	}
	else
	{
		ss << "<h2>commit [" << Id << "] (" << ShortTime << ")</h2>";
		for (auto &i : commits)
		{
			auto diff = (*i).GenerateDiffPatch(*this, markup);
			Implementation::ReplaceAll(diff, "\n", "<br>");
			ss << "<h3>parent commit " << (*i).Id << " (" << 
				(*i).ShortTime << ")</h3>" << diff << "<br>";
		}
	}

	return ss.str();
}


Nena::Git::Revwalk::String
Nena::Git::Revwalk::Commit::GenerateDesc(
_In_opt_ BOOL markup
) const
{
	std::stringstream ss;
	if (!markup)
		ss << "Time:\t" << LongTime << "\n",
		ss << "Id:\t" << Id << "\n",
		ss << "Msg:\t" << Message << "\n",
		ss << "By:\t" << AuthorName << "(" <<
		AuthorEmail << ")" << "\n";
	else
		ss << "<big>" << LongTime << "</big><br><br>",
		ss << "<b>" << Message << "</b><br><br>",
		ss << "sha " << Id << "<br>",
		ss << "by  " << AuthorName,
		ss << " (" << AuthorEmail << ")";

	return ss.str();
}


Nena::Git::Revwalk::Commit::List::const_reverse_iterator Nena::Git::Revwalk::crbegin() const
{
	return impl->crbegin();
}

Nena::Git::Revwalk::Commit::List::const_reverse_iterator Nena::Git::Revwalk::crend() const
{
	return impl->crend();
}

Nena::Git::Revwalk::Commit::List::const_iterator Nena::Git::Revwalk::cbegin() const
{
	return impl->cbegin();
}

Nena::Git::Revwalk::Commit::List::const_iterator Nena::Git::Revwalk::cend() const
{
	return impl->cend();
}
