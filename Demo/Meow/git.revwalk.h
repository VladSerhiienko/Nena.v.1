#pragma once

#include "app.precompiled.h"
#include "Nena\ReignTypedEvent.hxx"

namespace Nena
{
	namespace Git
	{
		using namespace Reign::Foundation;

		struct Revwalk
		{
			struct Implementation;

			typedef void *Unknown;
			typedef std::string String;
			typedef concurrency::task<void> ActionAsync;

			typedef struct Commit 
			{
				typedef std::list<Commit *> List;

				String Id; // sha
				String Message;
				SYSTEMTIME Time;
				String ShortTime;
				String LongTime;
				String AuthorName;
				String AuthorEmail;
				List Parents;

				/**!
				* Generates short commit description
				* @param [in] markup description with html tags
				* @returns short commit description
				*/
				String GenerateDesc(
					_In_opt_ BOOL markup = TRUE
					) const;

				/**!
				 * Generates a diff patch against parent commit 
				 * @param parent parent commit index  
				 * @returns generated patch text 
				 * @note if parent equals UINT_MAX, patch will be generated for all parent commits
				 */
				String GenerateDiffPatch(
					_In_opt_ UINT32 parent = UINT_MAX,
					_In_opt_ BOOL markup = TRUE
					) const;

				/**!
				* Generates a diff patch against other commit
				* @param commit other commit for diff generation
				* @returns generated patch text
				*/
				String GenerateDiffPatch(
					_In_ Commit const &commit,
					_In_opt_ BOOL markup = TRUE
					) const;

				/**!
				* Generates a diff patch against commits` collection
				* @param commits commits` collection for diff generation
				* @returns generated patch text
				*/
				String GenerateDiffPatch(
					_In_ List const &commits,
					_In_opt_ BOOL markup = TRUE
					) const;

			protected:
				friend Implementation;

				Commit(Revwalk *);
				Commit(Commit const &);
				~Commit();

				//! @note internal usage

				std::time_t time; 

				Revwalk *host; 
				Unknown *handle; 
				Unknown *treeHandle; 
				INT32 parentsExpected;
			};

			struct StatusUpdatedEventArgs 
			{
				Revwalk::String Status; //! updated status
			};

			typedef TypedEvent<Revwalk, StatusUpdatedEventArgs> StatusUpdatedEvent;

			/**!
			 * Tries to find a git repository from the location provided
			 * @param [in] startLocation location to start search from
			 * @param [out] gitLocation ./.git/ folder (if found)
			 * @returns zero if repository was found, error code otherwise 
			 */
			static BOOL DiscoverRepository(
				_In_ String const &startLocation,
				_Outref_ String &gitLocation
				);

			/**!
			* Processes repository asynchronously
			* @returns concurrent task
			*/
			ActionAsync RunAsync(
				_In_ String repository
				);

			/**!
			* Processes repository on caller thread
			*/
			void Run(
				_In_ String const &repository
				);

			Commit::List::const_reverse_iterator crbegin() const;
			Commit::List::const_reverse_iterator crend() const;
			Commit::List::const_iterator cbegin() const;
			Commit::List::const_iterator cend() const;
			Commit::List::size_type Size() const;

			Revwalk();
			~Revwalk();

			/**!
			 * Fires on each status change (e.g. when processing repository)
			 * Subsribe for this event to handle its hooks
			 */
			StatusUpdatedEvent StatusUpdated;
			//event StatusUpdatedEvent StatusUpdated;

		private:
			friend Implementation;
			Implementation *impl;
		};
	}
}