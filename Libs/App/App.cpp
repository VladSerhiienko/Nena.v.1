#include "App.h"
#include "AppVars.h"

Nena::Dispatcher::DefaultStub Nena::App::s_defaultStub;
Nena::Dispatcher::CaughtStub Nena::App::s_caughtStub;

Nena::App::App() : 
	DestroyEvent(Dispatcher::Message::Destroy), 
	CloseEvent(Dispatcher::Message::Close)
{
	DestroyEvent.Priority = 1100;
	CloseEvent.Priority = 1110;
	SetDefaults();
}

Nena::App::~App()
{
}

Nena::Dispatcher::Stub::Stub()
{
	OnDispatch = &Null;
	Priority = UINT_MAX;
}

Nena::Dispatcher::CaughtStub::CaughtStub()
{
	OnDispatch = &Null;
	Priority = UINT_MAX;
}

Nena::Dispatcher::LaunchEvent::LaunchEvent()
{
	Priority = 0;
	OnDispatch = &LaunchEvent::Fire;
}

void Nena::Dispatcher::Register(Event *e)
{
	if (Root) Root->Insert(e);
}

Nena::Dispatcher::Event::Event(UINT32 msg) : Msg(msg)
{
	OnDispatch = &DefaultDispatcherFast;
	OnMismatch = &App::s_defaultStub;
	OnFailed = &App::s_defaultStub;
	OnCaught = &App::s_caughtStub;
}

Nena::Dispatcher::MouseEvent::MouseEvent(UINT32 msg) : Event(msg)
{
	Parser = &MissingParser;
	SetPriority(50);
}

Nena::Dispatcher::KeyboardEvent::KeyboardEvent(UINT32 msg) : Event(msg)
{
	Parser = &MissingParser;
	SetPriority(100);
}

Nena::Dispatcher::QuitEvent::QuitEvent(UINT32 msg) : Event(msg)
{
	OnDispatch = &PostQuitMessageCallback;
	Priority = MaxPriority;
}

Nena::App::UserEvent::UserEvent() : Event(Nena::App::InterruptionMessage)
{
	Dispatcher::Event::OnDispatch = &Dispatch;
	Priority = MaxPriority + 5;
}


::LRESULT _Nena_DispatcherCallTy_ Nena::Dispatcher::ViewSizeChangedEvent::ViewSizeChanged(
	_In_ Nena::Dispatcher::Event *e,
	_In_::HWND hwnd, _In_::UINT32 msg,
	_In_::WPARAM wparam, _In_::LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		OutputDebugStringA("Nena::Dispatcher::ViewSizeChangedEvent::ViewSizeChanged()\n");
		auto app = App::GetForCurrentThread();
		app->View.SizeChanged(&app->View);
		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

Nena::Dispatcher::ViewSizeChangedEvent::ViewSizeChangedEvent()
	: Event(Nena::Application::Message::Size)
{
	Dispatcher::Event::OnDispatch = &ViewSizeChangedEvent::ViewSizeChanged;
	Priority = 1010;
}

LRESULT Nena::App::UserEvent::Dispatch(
	Event *e, HWND hwnd, UINT32 msg,
	WPARAM wparam, LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		OutputDebugStringA("Nena::App::UserEvent::Dispatch()\n");
		auto app = App::GetForCurrentThread();
		app->Interrupted(app);
		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

Nena::Dispatcher::PaintEvent::PaintEvent()
	: Event(Nena::Application::Message::Paint)
{
	Priority = 0; // MinPriority - 1;
}

Nena::Dispatcher::ViewResizedMovedEvent::ViewResizedMovedEvent()
	: Event(Nena::Application::Message::ExitSizeMove)
{
	Dispatcher::Event::OnDispatch = &ViewResizedMovedEvent::ExitSizeMove;
	Priority = 1010;
}

LRESULT _Nena_DispatcherCallTy_ Nena::Dispatcher::ViewResizedMovedEvent::ExitSizeMove(
	Event *e, HWND hwnd, UINT32 msg,
	WPARAM wparam, LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		OutputDebugStringA("Nena::App::ViewResizedMovedEvent::ExitSizeMove()\n");
		auto app = Nena::App::GetForCurrentThread();
		app->View.UpdateSizePosition();
		app->View.ResizedMoved(&app->View);
		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

void Nena::Dispatcher::Event::SetPriority(UINT32 priority)
{
	Priority = priority;
	ClampEventPriority(this);
}

UINT32 Nena::Dispatcher::Event::GetPriority()
{
	return Priority;
}

void Nena::Dispatcher::Event::Insert(
	Event *otherEvent
	)
{
	Event *thisEvent = this;
	Event *nextEvent = OnMismatch;

	if ((nextEvent->Priority) && 
		(nextEvent->Priority <= otherEvent->Priority))
		return nextEvent->Insert(otherEvent);
	else
		thisEvent->OnMismatch = otherEvent,
		otherEvent->OnMismatch = nextEvent;
}

void Nena::Dispatcher::Event::ClampEventPriority(Event *e)
{
	if (e->Priority > MaxPriority) e->Priority = MaxPriority;
	if (e->Priority < MinPriority) e->Priority = MinPriority;
}

Nena::Dispatcher::GenericArgs Nena::Dispatcher::MouseEvent::MissingParser(
	Event *e, HWND hwnd, UINT32 msg, WPARAM wparam, LPARAM lparam
	)
{
	GenericArgs mouse = { 0 };
	mouse.f32[0] = (FLOAT) GET_X_LPARAM(lparam);
	mouse.f32[1] = (FLOAT) GET_Y_LPARAM(lparam);
	return mouse;
}

Nena::Dispatcher::GenericArgs Nena::Dispatcher::KeyboardEvent::MissingParser(
	Event *e, HWND hwnd, UINT32 msg,	WPARAM wparam, LPARAM lparam
	)
{
	using namespace Nena::Application;

	GenericArgs keyboard = { 0 };

	keyboard.u32[0] = (UINT32) wparam;
	CorePhysicalKeyStatus::ExtractSlim(
		lparam, 
		keyboard.u32[1], 
		keyboard.u32[2]
		);

	return keyboard;
}

LRESULT Nena::Dispatcher::Event::DefaultDispatcherSafe(
	Event *e, HWND hwnd, UINT32 msg,
	WPARAM wparam, LPARAM lparam
	)
{
	if (msg == e->Msg && e->Callee && e->Parser)
	{
		if (e->Callee(e, e->Parser(
			e, hwnd, msg, wparam, lparam
			)))
		{
			if (e->OnCaught) return e->OnCaught->Dispatch(
				hwnd, msg, wparam, lparam
				);
			else return ::DefWindowProcA(
				hwnd, msg, wparam, lparam
				);
		}
		else
		{
			if (e->OnFailed) return e->OnFailed->Dispatch(
				hwnd, msg, wparam, lparam
				);
			else return ::DefWindowProcA(
				hwnd, msg, wparam, lparam
				);
		}
	}
	else
	{
		if (e->OnMismatch) return e->OnMismatch->Dispatch(
			hwnd, msg, wparam, lparam
			);
		else return ::DefWindowProcA(
			hwnd, msg, wparam, lparam
			);
	}
}

LRESULT Nena::Dispatcher::Event::DefaultDispatcherFast(
	Event *e, HWND hwnd, UINT32 msg,
	WPARAM wparam, LPARAM lparam
	)
{
	if (msg == e->Msg && e->Callee && e->Parser)
	{
		if (e->Callee(e, e->Parser(
			e, hwnd, msg, wparam, lparam))) 
			return e->OnCaught->Dispatch(
				hwnd, msg, wparam, lparam
				);
		else return e->OnFailed->Dispatch(
			hwnd, msg, wparam, lparam
			);
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

void Nena::App::MessageLoop(::BOOL withTimer)
{
	::OutputDebugStringA("Nena::App::MessageLoop()\n");
	::MSG e; ::ZeroMemory(&e, sizeof MSG);

	auto wnd = View.Raw;
	if (!wnd) return;

	if (OnInit) OnInit(this);
	MessageLoopStarted(this);

	if (withTimer && OnUpdate) while (OnUpdate)
	{
		if (PeekMessage(&e, wnd, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&e);
			::DispatchMessageA(&e);
		}

		if (e.message != WM_QUIT) { Timer.Tick([this] { OnUpdate(this); }); continue; }
		else OnUpdate = nullptr; // , QuitRequested(this);
	}
	else if (OnUpdate) while (OnUpdate)
	{
		if (::PeekMessageA(&e, wnd, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&e);
			::DispatchMessageA(&e);
		}

		if (e.message != WM_QUIT) { OnUpdate(this); continue; }
		else OnUpdate = nullptr; // , QuitRequested(this);
	}
	else while (true)
	{
		if (::PeekMessageA(&e, wnd, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&e);
			::DispatchMessageA(&e);
		}

		if (e.message == WM_QUIT)
			break;
	}

	MessageLoopQuit(this);
	if (OnQuit) OnQuit(this);
}

LRESULT Nena::Dispatcher::QuitEvent::PostQuitMessageCallback(
	Event *e, HWND hwnd, UINT32 msg,
	WPARAM wparam, LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		//if (msg == WM_QUIT)
		//{
		::OutputDebugStringA("Nena::App::QuitEvent::PostQuitMessageCallback()\n");
		App::GetForCurrentThread()->QuitRequested(
			App::GetForCurrentThread()
			);
		//}

		::PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	else
	{
		return e->OnMismatch->Dispatch(
			hwnd, msg, wparam, lparam
			);
	}
}


void Nena::App::SetDefaults()
{
	if (Handler.Root) return;

	::OutputDebugStringA("Nena::App::SetDefaults()\n");

	View.Style = Application::Window::DefaultWindowStyle;
	View.Fullscreen = FALSE;

	MessageLoopStarted += _Nena_Delegate_MakeBind(&App::AppendSizeMoveOnDemand, this);
	Launcher.Insert(&DestroyEvent);
	Launcher.Insert(&CloseEvent);
	Handler.Root = &Launcher;

	OnInit = &App::MissingInit;
	OnQuit = &App::MissingQuit;
}

void Nena::App::MissingInit(App *app)
{
	HRESULT result = S_OK;
	auto vars = Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	::OutputDebugStringA("Nena::App::MissingInit()\n");

	if (SUCCEEDED(result))
	{
		std::string windowClass;
		if (vars->HasRecord("App/View/Class")) windowClass = vars->GetRecord(("App/View/Class"));
		if (windowClass.size()) app->View.Name = windowClass;
	}

	app->View.Initialize(&App::Loop);

	if (SUCCEEDED(result))
	{
		std::string
			defaultIconFileSmall = "..\\..\\Bin\\@Icons\\cpp.ico",
			defaultIconFileBig = "..\\..\\Bin\\@Icons\\cpp.ico",
			text = "";

		Nena::Utilities::Neon::uint32x2_t 
			smallDims, bigDims;

		smallDims.u32[0] = 64;
		smallDims.u32[1] = 64;
		bigDims.u32[0] = 128;
		bigDims.u32[1] = 128;

		if (vars->HasRecord("App/View/Ico/Small/File")) defaultIconFileSmall = vars->GetRecord("App/View/Ico/Small/File");
		if (vars->HasRecord("App/View/Ico/Big/File")) defaultIconFileBig = vars->GetRecord("App/View/Ico/Big/File");
		if (vars->HasRecord("App/View/Ico/Small/Wpx")) smallDims.u32[0] = vars->GetRecord<UINT32>("App/View/Ico/Small/Wpx");
		if (vars->HasRecord("App/View/Ico/Small/Hpx")) smallDims.u32[1] = vars->GetRecord<UINT32>("App/View/Ico/Small/Hpx");
		if (vars->HasRecord("App/View/Ico/Big/Wpx")) bigDims.u32[0] = vars->GetRecord<UINT32>("App/View/Ico/Big/Wpx");
		if (vars->HasRecord("App/View/Ico/Big/Hpx")) bigDims.u32[1] = vars->GetRecord<UINT32>("App/View/Ico/Big/Hpx");
		if (vars->HasRecord("App/View/Text")) text = vars->GetRecord("App/View/Text");

		if (defaultIconFileSmall.size() || defaultIconFileBig.size())
			app->View.SetIcon(defaultIconFileSmall, defaultIconFileBig,
				smallDims.u32[0], smallDims.u32[1],
				bigDims.u32[0], bigDims.u32[1]);

		if (text.size()) app->View.SetTitle(text);
	}

}

void Nena::App::MissingQuit(App *app)
{
	::OutputDebugStringA("Nena::App::MissingQuit()\n");
	app->View.OnClosed();
}

void Nena::App::AppendSizeMoveOnDemand(_In_ App *app)
{
	::OutputDebugStringA("Nena::App::AppendSizeMoveOnDemand()\n");

	if ((View.Style & WS_THICKFRAME) == WS_THICKFRAME)
		Launcher.Insert(&ViewResizedMoved);
	else 
		Launcher.Insert(&ViewSizeChanged);
}

void Nena::App::EnableCrtChecks()
{
	::OutputDebugStringA("Nena::App::EnableCrtChecks()\n");
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
}

void Nena::App::PostUserMessage()
{
	::PostMessageA(
		View.Raw, Nena::App::InterruptionMessage, 
		NULL, (LPARAM) this
		);
}

void Nena::App::PostUserMessage(DWORD msg, WPARAM d, LPARAM p)
{
	::PostMessageA(
		View.Raw, msg, d, p
		);
}


