#include "app.precompiled.h"
#include "itv.engine.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_INCLUDED__

namespace InteractiveTV
{
	namespace Project
	{
		class Oasis
		{
			struct Implementation;
			friend Implementation;

		public:

			struct Web;
			struct Home;
			struct State;
			struct Shared;
			struct Object;
			struct Renderer;

			typedef std::string String;

			struct Object
			{
				friend ::InteractiveTV::Project::Oasis;

			protected: ::UINT16 Id; String Name;
			private: Object() = default;
			public: inline const char *GetNameCcstr() { return Name.c_str(); }
			public: inline String const &GetName() { return Name; }
			public: inline String GetNameCpy() { return Name; }
			};

			struct Shared : public Oasis::Object
			{
				::Nena::App *App = nullptr;
				::InteractiveTV::Engine *Engine = nullptr;
				::InteractiveTV::Project::Oasis *Oasis = nullptr;
				::InteractiveTV::Project::Oasis::Web *Web = nullptr;
				::InteractiveTV::Project::Oasis::String Uuid;
				::InteractiveTV::Project::Oasis::String Path;

			private:

				friend ::InteractiveTV::Project::Oasis;
				Shared();
			};

			struct State : public Oasis::Object
			{
				friend Oasis::Implementation;

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
				virtual void OnStateQuitted(_In_ Oasis::State *) = 0;
				virtual void OnStateResumed(_In_ Oasis::State *) = 0;
				virtual void OnStateSuspended(_In_ Oasis::State *) = 0;
				virtual void OnViewClosed(_In_ Oasis::State::View *) = 0;
				virtual void OnViewClosing(_In_ Oasis::State::View *) = 0;
				virtual void OnViewToggled(_In_ Oasis::State::View *) = 0;
				virtual void OnViewResizedMoved(_In_ Oasis::State::View *) = 0;

				State(_In_ Oasis::State *master);
				virtual ~State();

			private:

				Oasis::State::Event Destoyed;

			};

			__forceinline static Oasis *GetForCurrentThread();

			void AssignId(
				_In_ Oasis::Object *obj
				);

			void OnViewResizedMoved(
				_In_ ::Nena::Application::Window *
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
			static void SetName(
				_In_ Oasis::Object *master, 
				_In_ Oasis::Object *slave, 
				_In_ Oasis::String const &slave_name
				);

			void Run();
			void Quit();
			void OnQuit();
			void OnFrameMove();

			void OnKeyPressed(::UINT32 key);
			void OnKeyReleased(::UINT32 key);
			void OnMouseMoved(::UINT32 x, ::UINT32 y);
			void OnMouseLBPressed(::UINT32 x, ::UINT32 y);
			void OnMouseRBPressed(::UINT32 x, ::UINT32 y);
			void OnMouseLBReleased(::UINT32 x, ::UINT32 y);
			void OnMouseRBReleased(::UINT32 x, ::UINT32 y);

			Shared Context;

		private:

			Implementation *impl;

			Oasis();
			~Oasis();

			void OnAppQuitRequested(::Nena::App *);

			static void GenerateUuid();
			static ::BOOL CreateFolder(Oasis::String path);
			static void _Nena_AppCallbackObjCallTy_ OnUpdate(::Nena::App *);
		};

	}

#include "itv.oasis.inl"

}

#endif // !__NENA_INTERACTIVE_TV_OASIS_INCLUDED__
