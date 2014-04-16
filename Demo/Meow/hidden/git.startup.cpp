#include "app.precompiled.h"

#include <Windows.h>
#include <debugapi.h>

#include <QApplication>
#include "qt.dialog_mainwnd.ui.mo.h"
#include <iostream>
#include <ctime>

#include <git2.h>

int LaunchGit(int argc, char *argv []);

int main_git(int argc, char *argv[])
{
	return LaunchGit(argc, argv);
}

typedef struct { /* … */ } diff_data; 
typedef struct { /* … */ } walk_data;

int walk_cb(
	const char *root,
	const git_tree_entry *entry,
	void *payload
	)
{
	walk_data *d = (walk_data*) payload;

	auto name = git_tree_entry_name(entry);
	std::cout << name << std::endl;

	/* … */

	return 0;
}


int each_file_cb(
	const git_diff_delta *delta,
	float progress,
	void *payload
	)
{
	std::cout << ">> " << delta->new_file.path << std::endl;
	std::cout << ">> " << delta->old_file.path << std::endl;
	std::cout << ">> " << "------------------" << std::endl;
	return 0;
}

int each_hunk_cb(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	void *payload
	)
{



	return 0;
}

int each_line_cb(
	const git_diff_delta *delta,
	const git_diff_hunk *hunk,
	const git_diff_line *line,
	void *payload
	)
{

	return 0;
}

int LaunchGit(int argc, char *argv[])
{

	int error = 0;
	char const pathLookup[] = "D:/Development/VisualStudio/Aktuelle/Nena (Git) v1/Bin/";

	::git_buf pathBuf;
	::memset(&pathBuf, 0, sizeof pathBuf);
	::git_buf_grow(&pathBuf, 1024);

	::git_repository *repo = NULL;

	error = ::git_repository_discover( pathBuf.ptr, pathBuf.asize, pathLookup, false, NULL );

	if (error < 0)
	{
		const git_error *e = giterr_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);

	}
	else error = ::git_repository_open(&repo, pathBuf.ptr);

	if (error < 0)
	{
		const git_error *e = giterr_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);

	}

	//git_object *obj = NULL;
	//error = git_revparse_single(&obj, repo, "HEAD^{tree}");
	//git_tree *tree = (git_tree *) obj;

	//walk_data d;
	//error = git_tree_walk(tree, GIT_TREEWALK_PRE, walk_cb, &d);

	//git_tree_free(tree);

	git_revwalk *walker;
	error = git_revwalk_new(&walker, repo);
	error = git_revwalk_push_range(walker, "origin/master..HEAD");
	git_revwalk_sorting(walker, GIT_SORT_TIME);

	git_oid oid;
	::memset(&oid, 0, sizeof oid);
	while (!::git_revwalk_next(&oid, walker))
	{
		/* … */

		git_commit *commit = NULL;
		git_commit_lookup(&commit, repo, &oid);

		std::string msg = git_commit_message(commit);
		auto author = git_commit_author(commit);
		auto sha = git_oid_allocfmt(git_commit_id(commit));

		if (msg.back() == '\n')
			*(msg.end() - 1) = '\0';

		char stime[512];
		auto time = ctime_s(stime, sizeof stime, (time_t*) &author->when);
		printf(
			"commit  sha %s:\n\tmessage %s\n\tauthor %s > %s\n\ttime %s\n", 
			sha, msg.c_str(), author->name, author->email, stime
			);

		auto parentCount = git_commit_parentcount(commit);
		for (uint32_t parentIndex = 0; parentIndex < parentCount; parentIndex++)
		{
			git_commit 
				*parent = 0;

			git_tree 
				*thisTree = 0, 
				*parentTree = 0;

			error = git_commit_parent(&parent, commit, parentIndex);
			error = git_commit_tree(&thisTree, commit);
			error = git_commit_tree(&parentTree, parent);

			git_diff *diff = 0;
			error = git_diff_tree_to_tree(&diff, repo, thisTree, parentTree, 0);

			diff_data d;
			error = git_diff_foreach(diff,
				each_file_cb,
				each_hunk_cb,
				each_line_cb,
				&d);

			git_patch *patch = NULL;
			error = git_patch_from_diff(&patch, diff, 0);

			char *str;
			error = git_patch_to_str(&str, patch);
			OutputDebugStringA(str);
			OutputDebugStringA("\n");
		}

		//free(sha);
		git_commit_free(commit);
	}

	git_repository_free(repo);
	git_buf_free(&pathBuf);


	//return LaunchQt(argc, argv);
	getchar();
	return EXIT_SUCCESS;
}


int LaunchQt(int argc, char *argv[])
{
	QApplication app(argc, argv);
	MainWindowDialog dialog;
	dialog.show();
	return app.exec();
}