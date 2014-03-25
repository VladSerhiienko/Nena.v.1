#include <Windows.h>
#include <Nena\App.h>
#include <Nena\AppVars.h>
#include <Nena\PlatformAssist.h>

static void __fastcall AppUpdate(_In_ Nena::App *app)
{
}

static LRESULT __fastcall KeyboardDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		Nena::Dispatcher::GenericArgs args = e->Parser(
			e, hwnd, msg, wparam, lparam
			);

		if (args.u32[0] == Nena::Application::VirtualKey::F) Nena::Platform::DebugTrace(
				"fc: %f fps: %u\n", 
				Nena::App::GetForCurrentThread()->GetTimer()->GetElapsedSeconds(),
				Nena::App::GetForCurrentThread()->GetTimer()->GetElapsedTicks()
				);
		else Nena::Platform::DebugTrace(
			"address=0x%08x > key=%u\n", e,
			args.u32[0]
			);

		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

static LRESULT __fastcall MouseDispatchCallback(
	_In_ Nena::Dispatcher::Event *e, 
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	if (e->Msg == msg)
	{
		Nena::Dispatcher::GenericArgs args = e->Parser(
			e, hwnd, msg, wparam, lparam
			);
		Nena::Platform::DebugTrace(
			"address=0x%08x > [x=%3.1f y=%3.1f]\n", e,
			args.f32[0],
			args.f32[1]
			);
		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

INT32 WINAPI WinMain(
	::HINSTANCE hInstance, 
	::HINSTANCE hPrevInstance, 
	::LPTSTR pScmdline, 
	::INT32 iCmdshow
	)
{
	Nena::App *app = Nena::App::GetForCurrentThread();

	app->EnableCrtChecks();
	app->SetDefaults();

	Nena::Dispatcher::MouseEvent mouseMoveEvent(Nena::Application::MouseMove);
	mouseMoveEvent.OnDispatch = &MouseDispatchCallback;
	mouseMoveEvent.SetPriority(10);

	Nena::Dispatcher::KeyboardEvent keyPressed(Nena::Application::KeyPressed);
	keyPressed.OnDispatch = &KeyboardDispatchCallback;
	keyPressed.SetPriority(20);

	app->Handler.Register(&mouseMoveEvent);
	app->Handler.Register(&keyPressed);
	app->OnUpdate = &AppUpdate;

	app->MessageLoop();

	return 0;
}