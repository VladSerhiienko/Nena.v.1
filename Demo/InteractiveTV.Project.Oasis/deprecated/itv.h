#pragma once

#include "app.precompiled.h"
#include "Delegate.h"

#include <Nena\AppNeon.h>
#include <Nena\Window.h>
#include <Nena\PlatformAssist.h>

namespace Nena
{
	/**! 
	* @details 
	*	Controls interactive TV
	*/
	class InteractiveTV
	{
	public:

		/**! 
		 * @details    
		 *	manages the lifetime of all the web views and  
		 *	maintains useful services like resource caching
		 *	and network connections
		 */
		class Web
		{
			friend InteractiveTV;
			struct Implementation;
			friend Implementation;
			Implementation *impl;

			void Initialize();
			void Release();

		public:

			class View;
			class Session;

		public:

			/**! 
			 * @details 
			 *	loads web pages, interacts with it, renders it on-the-fly
			 *	to the surface (can be created using @c Web)
			 */
			class View 
			{
				struct Implementation;

				friend Web;
				friend Session;
				friend Implementation;

				typedef unsigned __int32 uint32;
				typedef Nena::Utilities::Neon::uint32x2_t uint32x2;

				Web *core;
				uint32x2 dims;
				Implementation *impl;

			public:
				class Notify;
				class Surface;
				typedef std::vector<View *> Vector;

				class Surface
				{
					//! buffer 
					//! dims
				};

				class Notify
				{
				public:

					class LoadEventArgs;
					class ProcessEventArgs;
					typedef Nena::Event<void, Nena::InteractiveTV::Web::View *, LoadEventArgs *> LoadEvent;
					typedef Nena::Event<void, Nena::InteractiveTV::Web::View *, ProcessEventArgs *> ProcessEvent;

					typedef class ProcessEventArgs
					{
						friend Implementation;
						friend View::Implementation;
						~ProcessEventArgs();

					public:

						union
						{
							::BOOL responsiveness_state;
							::UINT32 termination_status;
						};
					};

					typedef class LoadEventArgs
					{
						friend Implementation;
						friend View::Implementation;
						~LoadEventArgs();

					public:

						INT64 frame_id;
						BOOL is_main_frame;
						INT32 error_code;
						INT32 is_error_page;
						std::string url;

					};

					ProcessEvent ResponsivenessChanged;
					ProcessEvent Crashed;
					LoadEvent LoadingStarted;
					LoadEvent LoadingFinished;
					LoadEvent LoadingFailed;
					LoadEvent DocumentBuilt;
				};

			public:

				Notify notify;

			public:

				View(Web *core);
				~View();

				void Initialize(
					_In_ Nena::Application::Window *window, 
					_In_ Session *session
					);

				void Release();

				void GetDims(
					_Out_ View::uint32 &view_width,
					_Out_ View::uint32 &view_height
					);

				void LoadURL(_In_ std::string const &url);
				BOOL IsLoading(); //! @todo 

			};

		public:

			/**! 
			 * @details 
			 *	is responsible for storing all user-generated data (cookies, cache, 
			 *	authentification, etc), it can either be purely inmemory or saved to 
			 *	disk (a writtable path should be provided to store data at runtime)
			 */
			class Session
			{
				friend Web;
				friend View;
				struct Implementation;
				friend Implementation;

				Web *core;
				Implementation *impl;

			public:

				typedef std::vector<Session *> Vector;

			public:

				Session(Web *core);
				~Session();
				void Initialize();
				void Release();

			};

		public:

			Web();
			~Web();

			Session *GetSession();  //! @todo remove
			View *GetView();  //! @todo remove

			void Update();  //! @todo implement

		};

	public:

		Web online;

	public:

		InteractiveTV();
		~InteractiveTV();

	private:

		void Initialize();
		void Release();


	};
}
