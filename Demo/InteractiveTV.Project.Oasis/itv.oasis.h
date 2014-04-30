#include "app.precompiled.h"
#include "itv.engine.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_INCLUDED__

#define _Oasis_origin "/.//oasis/"

namespace InteractiveTV
{
	namespace Project
	{
		class Oasis
		{
			struct Implementation;
			friend Implementation;

		public:

			struct Air;
			struct Web;
			struct Home;
			struct State;
			struct Shared;
			struct Object;

			typedef std::string String;

			struct Object
			{
				friend ::InteractiveTV::Project::Oasis;

			protected: Object( ) = default;
			protected: ::UINT16 Id; String Name;
			public: inline String GetNameCpy( ) { return Name; }
			public: inline String const &GetName( ) { return Name; }
			public: inline const char *GetNameCcstr( ) { return Name.c_str( ); }
			};

			struct Shared : public Oasis::Object
			{
				::Nena::App *App = nullptr;
				::Nena::Simulation::BasicTimer Timer;
				::InteractiveTV::Engine *Engine = nullptr;
				::InteractiveTV::Project::Oasis *Oasis = nullptr;
				::InteractiveTV::Project::Oasis::Web *Web = nullptr;
				::InteractiveTV::Project::Oasis::Air *Air = nullptr;
				::InteractiveTV::Project::Oasis::String Uuid;
				::InteractiveTV::Project::Oasis::String Path;
				::InteractiveTV::Project::Oasis::String UniquePath;

			private:

				friend ::InteractiveTV::Project::Oasis;
				Shared();
			};

			struct State : public Oasis::Object
			{
				friend Oasis::Implementation;

				typedef ::std::vector<Oasis::State *> List;
				typedef ::Nena::Application::Window View;
				typedef ::Nena::Event<void, Oasis::State *> Event;
				typedef ::Nena::Delegate<void, Oasis::State *> Delegate;
				typedef ::Nena::Delegate<void, Oasis::State::View *> ViewDelegate;

				Oasis::State *Master;
				Oasis::Shared *Context;						//! cross oasis data
				Oasis::State::Event Initted;				//! emits on initialized
				Oasis::State::Event Quitted;				//! emits on uninitialized
				Oasis::State::Event Resumed;				//! emits on resuming 
				Oasis::State::Event Suspended;				//! emits on toggling idle mode
				Oasis::State::Event ResourcesCreated;		//! emits, when all the resources are ready
				Oasis::State::Event ResourcesDestroyed;		//! emits, when all the resources are released

				virtual void Init() = 0;
				virtual void Quit() = 0;
				virtual void Resume() = 0;
				virtual void Suspend() = 0;
				virtual void OnStateQuitted(_In_ Oasis::State *) {}
				virtual void OnStateResumed(_In_ Oasis::State *) {}
				virtual void OnStateSuspended(_In_ Oasis::State *) {}

				State(_In_ Oasis::State *master);
				virtual ~State();

			private:

				Oasis::State::Event Destoyed;

			};

			__forceinline static Oasis *GetForCurrentThread();

			void AssignId(
				_In_ Oasis::Object *obj
				);

			void OnStateQuitted(
				_In_ Oasis::State *state
				);
			void OnStateResumed(
				_In_ Oasis::State *state
				);
			void OnStateSuspended(
				_In_ Oasis::State *state
				);
			void OnStateDestroyed(
				_In_ Oasis::State *state
				);

			void Run();
			void Quit();
			void OnQuit();
			void OnFrameMove();
			void OnToggleFullscreen();

			void OnKeyPressed(_In_::UINT32 key);
			void OnKeyReleased(_In_::UINT32 key);
			void OnMouseMoved(_In_::FLOAT x, _In_::FLOAT y);
			void OnMouseLBPressed(_In_::FLOAT x, _In_::FLOAT y);
			void OnMouseRBPressed(_In_::FLOAT x, _In_::FLOAT y);
			void OnMouseLBReleased(_In_::FLOAT x, _In_::FLOAT y);
			void OnMouseRBReleased(_In_::FLOAT x, _In_::FLOAT y);
			void OnSizeChanged(_In_::USHORT x, _In_::USHORT y);

			Shared Context;

		public:

			static void GenerateUuid(Oasis::String &path);
			static ::BOOL CreateFolder(Oasis::String path);

		private:

			Implementation *impl;

			Oasis();
			~Oasis();

			void OnAppQuitRequested(::Nena::App *);

			void OnDeviceLost(::Nena::Graphics::DeviceResources *);
			void OnDeviceRestored(::Nena::Graphics::DeviceResources *);
			void OnSwapchainResizing(::Nena::Graphics::DeviceResources *);
			void OnSwapchainResized(::Nena::Graphics::DeviceResources *);

		private:
		};

	}

#include "itv.oasis.inl"

}

#endif // !__NENA_INTERACTIVE_TV_OASIS_INCLUDED__
