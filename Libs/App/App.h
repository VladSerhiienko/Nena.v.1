/*** 

\author Vlad Serhiienko    
\date 2/3/2014 4:59am    
 
\brief 
	A bunch of classes designed for real-time applications using Win32 API.   
 
\details  
	Those classes are implemented without virtual functions,   
	but still are very flexible and easy to use and to understand.   
	App events are designed so that one can handle very complex pipelines,   
	combine those pipelines, make changes in run-time without loosing robustness, 
	design different scenarios for app, that will execute seamlessly and fast.

*/

#include <Windows.h>
#include <Nena\Window.h>
#include <Nena\StepTimer.h>
#include <Nena\Delegate.h>
#include <ppl.h>
#include <ppltasks.h>

#include "AppNeon.h"
#include "AppEventAssets.h"

#ifndef __NENA_APP_INCLUDED__
#define __NENA_APP_INCLUDED__

#define _Nena_DispatcherCallbackObjCallTy_			__vectorcall
#define _Nena_AppCallbackObjCallTy_					__fastcall
#define _Nena_DispatcherCallTy_						__fastcall

namespace Nena
{
	struct App;
	struct Dispatcher
	{
		struct Event;
		typedef void *Unknown;
		typedef ::Nena::Application::Message Message;
		typedef ::Nena::Utilities::Neon::N128 GenericArgs;

		typedef GenericArgs(_Nena_DispatcherCallTy_*ExtractObj)(_In_ Event *, _In_::HWND, _In_::UINT32, _In_::WPARAM, _In_::LPARAM);
		typedef ::LRESULT(_Nena_DispatcherCallTy_*DispatchObj)(_In_ Event *, _In_::HWND, _In_::UINT32, _In_::WPARAM, _In_::LPARAM);
		typedef ::BOOL(_Nena_DispatcherCallTy_*CallbackObj)(_In_ Event *, _In_ GenericArgs const &);

		typedef struct Event
		{
			friend App;
			friend Dispatcher;

			static const ::UINT32 MinPriority = 5u;
			static const ::UINT32 MaxPriority = 1000u;

			static ::LRESULT _Nena_DispatcherCallTy_ DefaultDispatcherSafe(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);
			static LRESULT _Nena_DispatcherCallTy_ DefaultDispatcherFast(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);
			static void ClampEventPriority(
				_In_ Event *e
				);

			Event(_In_::UINT32 msg = Message::Other); // msg ctor
			void Insert(_In_ Event *other);

			void SetPriority(::UINT32 priority);
			::UINT32 GetPriority();

			__forceinline ::LRESULT _Nena_DispatcherCallTy_ Dispatch(
				_In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				)
			{
				return OnDispatch(
					this, hwnd, msg,
					wparam, lparam
					);
			}

			::UINT32 Msg = Message::Other; // window message to react on
			DispatchObj OnDispatch = nullptr; // is called in dispatch method
			CallbackObj Callee = nullptr; // call this if event is caught
			ExtractObj Parser = nullptr; // call this when event should be parsed
			Event *OnMismatch = nullptr; // call this if the msg cannot be processed by this handler
			Event *OnFailed = nullptr; // call this if event Callee(...) call failed (wrong data, etc)
			Event *OnCaught = nullptr; // call this if event is processed
			Dispatcher *Host = nullptr;

		private:

			::UINT32 Priority = MinPriority;
		} Event, BasicEvent;

		typedef struct MouseEvent : public Event
		{
			static GenericArgs _Nena_DispatcherCallTy_ MissingParser(
				_In_ Event *e,
				_In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);

			MouseEvent(_In_::UINT32 msg = Message::Other);
		} MouseEvent;

		typedef struct KeyboardEvent : public Event
		{
			static GenericArgs _Nena_DispatcherCallTy_ MissingParser(
			_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);

			KeyboardEvent(_In_::UINT32 msg = Message::Other);
		} KeyboardEvent;

		typedef struct LaunchEvent : public Event
		{
			__forceinline static ::LRESULT _Nena_DispatcherCallTy_ Fire(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				)
			{
				return e->OnMismatch->OnDispatch(
					e->OnMismatch, 
					hwnd, msg, 
					wparam, 
					lparam
					);
			}

			LaunchEvent();
		} LaunchEvent, Scenario;

		typedef struct CaughtStub : public Event
		{
			friend App;
		private:

			__forceinline static ::LRESULT _Nena_DispatcherCallTy_ Null(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);

			CaughtStub();
		} CaughtStub;

		typedef struct Stub : public Event
		{
			friend App;
		private:

			__forceinline static ::LRESULT _Nena_DispatcherCallTy_ Null(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);

			Stub();
		} DefaultStub;

		__forceinline ::LRESULT _Nena_DispatcherCallTy_ Dispatch(
			_In_::HWND hwnd, _In_::UINT32 msg,
			_In_::WPARAM wparam, _In_::LPARAM lparam
			);

		void Register(_In_ Event *e);

		Dispatcher::LaunchEvent *Root = nullptr;
		Dispatcher::Unknown UserData = nullptr;
	};

	struct App
	{
		struct Vars;
		struct Talk;
		friend Dispatcher::Event;

		typedef void *Unknown;
		typedef ::HWND HWindow;
		typedef void(_Nena_AppCallbackObjCallTy_*AppCallbackObj1)(_In_::Nena::App::Unknown);
		typedef void(_Nena_AppCallbackObjCallTy_*AppCallbackObj2)(_In_::Nena::App *);
		typedef AppCallbackObj2 AppCallbackObj;
		typedef ::Nena::Delegate<void, ::Nena::App *> AppHandler;
		typedef ::Nena::Event<void, ::Nena::App *> AppEvent;


		struct PaintEvent : public Dispatcher::Event
		{
			PaintEvent();
		};

		struct ViewResizedMovedEvent : public Dispatcher::Event
		{
			static ::LRESULT _Nena_DispatcherCallTy_ ExitSizeMove(
				_In_ Nena::Dispatcher::Event *e,
				_In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);
			ViewResizedMovedEvent();
		};

		// Can be used to handle Dispatcher::Message::Close and 
		// Dispatcher::Message::Destroy events.
		struct QuitEvent : public Dispatcher::Event
		{
			static ::LRESULT _Nena_DispatcherCallTy_ PostQuitMessageCallback(
				_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
				_In_::WPARAM wparam, _In_::LPARAM lparam
				);

			QuitEvent(_In_::UINT32 msg);
		};

		struct UserEvent : public Dispatcher::Event
		{
			static ::LRESULT _Nena_DispatcherCallTy_ Dispatch(
			_In_ Event *e, _In_::HWND hwnd, _In_::UINT32 msg,
			_In_::WPARAM wparam, _In_::LPARAM lparam
			);

			UserEvent();
		};

		static void _Nena_AppCallbackObjCallTy_ MissingInit(_In_ App *app);
		static void _Nena_AppCallbackObjCallTy_ MissingQuit(_In_ App *app);

		void EnableCrtChecks();
		void MessageLoop(::BOOL withTimer = TRUE);
		void PostUserMessage();
		void PostUserMessage(::DWORD msg, ::WPARAM, ::LPARAM);
		void SetDefaults();

		App();
		~App();

		static const ::DWORD InterruptionMessage = WM_USER + 1;

		AppCallbackObj OnUpdate = nullptr;
		AppCallbackObj OnInit = nullptr;  //! @deprecated @see event MessageLoopStarted
		AppCallbackObj OnQuit = nullptr;  //! @deprecated @see event MessageLoopQuit
		Unknown UserData = nullptr;

		Simulation::StepTimer Timer;
		Application::Window View;

		App::ViewResizedMovedEvent ViewResizedMoved;
		App::QuitEvent DestroyEvent;
		App::QuitEvent CloseEvent;
		AppEvent MessageLoopStarted;
		AppEvent MessageLoopQuit;
		AppEvent QuitRequested;
		AppEvent Interrupted;

		Dispatcher::LaunchEvent Launcher;
		Dispatcher Handler;

		__forceinline static ::Nena::App *GetForCurrentThread();

		__forceinline static ::LRESULT CALLBACK Loop(
			_In_::HWND hwnd,
			_In_::UINT msg,
			_In_::WPARAM wparam,
			_In_::LPARAM lparam
			);

		template <typename _TyCallback>
		__forceinline static void CoWait(
			_In_::concurrency::task<void> task,
			_In_ _TyCallback callback, 
			_In_opt_::BOOL timer = TRUE,
			_In_opt_::LPCSTR name = "_nena_app_cowait_event"
			);

		__forceinline Simulation::StepTimer const *GetTimer() const;
		__forceinline Simulation::StepTimer *GetTimer();

	protected:

		static Dispatcher::DefaultStub s_defaultStub;
		static Dispatcher::CaughtStub s_caughtStub;
		void AppendSizeMoveOnDemand(_In_ App *app);


	};

#include "App.inl"
}

#endif