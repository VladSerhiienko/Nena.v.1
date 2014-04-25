#include "Engine.h"
#include "ShadowMapping.h"

void Demo::EngineNotify::OnDeviceLost() { Engine::GetForCurrentThread()->OnDeviceLost(); }
void Demo::EngineNotify::OnDeviceRestored() { Engine::GetForCurrentThread()->OnDeviceRestored(); }

void Demo::Engine::OnDeviceLost() {}

void Demo::Engine::OnDeviceRestored() {}

Demo::Input::Input() : 
	KeyPressed(Nena::Application::Message::KeyPressed),
	KeyReleased(Nena::Application::Message::KeyReleased),
	MouseMoved(Nena::Application::Message::MouseMove),
	MouseLeftButtonPressed(Nena::Application::Message::LeftButtonPressed),
	MouseLeftButtonReleased(Nena::Application::Message::LeftButtonReleased),
	MouseRightButtonPressed(Nena::Application::Message::RightButtonPressed),
	MouseRightButtonReleased(Nena::Application::Message::RightButtonReleased)
{

	MouseMoved.SetPriority(10);
	MouseLeftButtonPressed.SetPriority(20);
	MouseLeftButtonReleased.SetPriority(20);
	MouseRightButtonPressed.SetPriority(20);
	MouseRightButtonReleased.SetPriority(20);
	KeyPressed.SetPriority(30);
	KeyReleased.SetPriority(30);

	KeyPressed.OnDispatch = &KeyboardDispatchCallback;
	KeyReleased.OnDispatch = &KeyboardDispatchCallback;
	MouseLeftButtonPressed.OnDispatch = &MouseDispatchCallback;
	MouseLeftButtonReleased.OnDispatch = &MouseDispatchCallback;
	MouseRightButtonPressed.OnDispatch = &MouseDispatchCallback;
	MouseRightButtonReleased.OnDispatch = &MouseDispatchCallback;

	Nena::App::GetForCurrentThread()->Handler.Register(&KeyPressed);
	Nena::App::GetForCurrentThread()->Handler.Register(&KeyReleased);
	Nena::App::GetForCurrentThread()->Handler.Register(&MouseMoved);
	Nena::App::GetForCurrentThread()->Handler.Register(&MouseLeftButtonPressed);
	Nena::App::GetForCurrentThread()->Handler.Register(&MouseLeftButtonReleased);
	Nena::App::GetForCurrentThread()->Handler.Register(&MouseRightButtonPressed);
	Nena::App::GetForCurrentThread()->Handler.Register(&MouseRightButtonReleased);
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
	static ShadowMapping *demo = ShadowMapping::Get();

	demo->Update();
	demo->Render();
}

LRESULT _Nena_DispatcherCallTy_ Demo::Input::KeyboardDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static Nena::Simulation::StepTimer *timer = 
		Nena::App::GetForCurrentThread()->GetTimer();

	if (e->Msg == msg)
	{
		Nena::Dispatcher::GenericArgs args = e->Parser(e, hwnd, msg, wparam, lparam);
		if (args.u32[0] == Nena::Application::VirtualKey::F) Nena::Platform::DebugTrace(
			"fc: %f fps: %u\n", timer->GetElapsedSeconds(), timer->GetElapsedTicks());
		else if (args.u32[0] == Nena::Application::VirtualKey::Escape) 
			::PostQuitMessage(EXIT_SUCCESS);
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
	static ShadowMapping *demo = ShadowMapping::Get();
	Nena::Dispatcher::GenericArgs args = e->Parser(e, hwnd, msg, wparam, lparam);

	switch (msg)
	{

	case Nena::Application::Message::MouseMove: 

		if (demo->MLBPressed)
		{
			demo->MLDlt.x = args.f32[0] - demo->MLPos.x;
			demo->MLDlt.y = args.f32[1] - demo->MLPos.y;
			demo->Camera.UpdateView(demo->MLDlt, 0);
		}

		if (demo->MRBPressed)
		{
			demo->MRDlt.x = args.f32[0] - demo->MRPos.x;
			demo->MRDlt.y = args.f32[1] - demo->MRPos.y;
			demo->Light.UpdateView(demo->MRDlt, 0);
		}

		demo->MLPos.x = args.f32[0];
		demo->MLPos.y = args.f32[1];
		demo->MRPos.x = args.f32[0];
		demo->MRPos.y = args.f32[1];

		break;
	case Nena::Application::Message::LeftButtonPressed: 
		demo->MLBPressed = TRUE;
		break;
	case Nena::Application::Message::LeftButtonReleased:
		demo->MLBPressed = FALSE;
		break;
	case Nena::Application::Message::RightButtonPressed:
		demo->MRBPressed = TRUE;
		break;
	case Nena::Application::Message::RightButtonReleased:
		demo->MRBPressed = FALSE;
		break;
	default: return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
	}

	return 0;
}
