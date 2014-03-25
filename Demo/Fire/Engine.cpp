#include "Engine.h"

void Demo::EngineNotify::OnDeviceLost() { Engine::GetForCurrentThread()->OnDeviceLost(); }
void Demo::EngineNotify::OnDeviceRestored() { Engine::GetForCurrentThread()->OnDeviceRestored(); }

void Demo::Engine::OnDeviceLost() {}

void Demo::Engine::OnDeviceRestored() {}

Demo::Input::Input() : 
	KeyPressed(Nena::Application::Message::KeyPressed),
	KeyReleased(Nena::Application::Message::KeyReleased)
{
	KeyPressed.SetPriority(50);
	KeyReleased.SetPriority(60);
	KeyPressed.OnDispatch = &KeyboardDispatchCallback;
	KeyReleased.OnDispatch = &KeyboardDispatchCallback;

	Nena::App::GetForCurrentThread()->Handler.Register(&KeyPressed);
	Nena::App::GetForCurrentThread()->Handler.Register(&KeyReleased);
}

Demo::Graphics::Graphics() : D3D(D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE)
//Demo::Graphics::Graphics() : D3D(D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE)
{
	Nena::App::GetForCurrentThread()->EnableCrtChecks();
	Nena::App::GetForCurrentThread()->SetDefaults();
	Nena::App::GetForCurrentThread()->OnInit = nullptr;
	Nena::App::MissingInit(Nena::App::GetForCurrentThread());

	D3D.Vsync = TRUE;
	D3D.Windowed = !Nena::App::GetForCurrentThread()->View.Fullscreen;
	D3D.SetWindow(Nena::App::GetForCurrentThread()->View.Raw);

	D2D.CreateDeviceIndependentResources();
	D2D.CreateDeviceResources(D3D.Device.Get());
	D2D.SetDpi(D3D.Dpi.x, D3D.Dpi.y);
	D2D.CreateWindowSizeDependentResources(D3D.Swapchain.Get());
}

Demo::Graphics::~Graphics()
{

}

Demo::Engine::Engine() : Graphics(), Input(), Notify()
{
	Nena::App::GetForCurrentThread()->OnUpdate = &OnUpdate;
	Nena::App::Vars::GetForCurrentThread()->Read();
	Graphics::D3D.RegisterDeviceNotify(&Notify);
}

void _Nena_AppCallbackObjCallTy_ Demo::Engine::OnUpdate(Nena::App *app)
{
	static Demo::FeuerFrei *fire = Demo::Engine::GetForCurrentThread()->Fire;
	fire->Update();
	fire->Render();
}

LRESULT _Nena_DispatcherCallTy_ Demo::Input::KeyboardDispatchCallback(
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
		else if (args.u32[0] == Nena::Application::VirtualKey::Escape)
			::PostQuitMessage(0);

		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ Demo::Input::MouseDispatchCallback(
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
