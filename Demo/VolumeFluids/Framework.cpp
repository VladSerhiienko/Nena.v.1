#include "Framework.h"

#define __Nena_Framework_Discard_timer_updates 1


namespace Framework
{
	namespace Application
	{
		static LRESULT CALLBACK Loop(
			_In_::HWND hwnd,
			_In_::UINT umsg,
			_In_::WPARAM wparam,
			_In_::LPARAM lparam
			)
		{
			static Window *window = Window::GetForCurrentThread();
			return window->EventHandler.Process(hwnd, umsg, wparam, lparam);
		}
	}
}

#pragma region Device norify implementation

Framework::Application::WindowDeviceNotify::WindowDeviceNotify(
	Framework::Application::Window *host
	) : Host(host)
{
}

void Framework::Application::WindowDeviceNotify::OnDeviceLost()
{
	OutputDebugString("OnDeviceLost\n");
}

void Framework::Application::WindowDeviceNotify::OnDeviceRestored()
{
	OutputDebugString("OnDeviceRestored\n");
}

#pragma endregion

#pragma region Window event dispatcher implementation

Framework::Application::Dispatcher::Dispatcher(
	Framework::Application::Window *host
	) : Host(host)
{
}

LRESULT Framework::Application::Dispatcher::Process(
	_In_::HWND hwnd,
	_In_::UINT umsg,
	_In_::WPARAM wparam,
	_In_::LPARAM lparam
	)
{
	switch (umsg)
	{
	case WM_MOUSEMOVE:
		return Host->OnMouseMoved(
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
			);
	case WM_LBUTTONDOWN:
		return Host->OnMouseLeftButtonPressed(
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
			);
	case WM_LBUTTONUP:
		return Host->OnMouseLeftButtonReleased(
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
			);
	case WM_RBUTTONDOWN:
		return Host->OnMouseRightButtonPressed(
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
			);
	case WM_RBUTTONUP:
		return Host->OnMouseRightButtonReleased(
			GET_X_LPARAM(lparam),
			GET_Y_LPARAM(lparam)
			);
	case WM_KEYDOWN:
		return Host->OnKeyPressed(
			(UINT16) wparam
			);

	case WM_CLOSE:
		::OutputDebugStringA("WM_CLOSE\n");
		::PostQuitMessage(EXIT_SUCCESS);
		return 0;

	case WM_DESTROY:
		::OutputDebugStringA("WM_DESTROY\n");
		::PostQuitMessage(EXIT_SUCCESS);
		return 0;
	case WM_EXITSIZEMOVE:
		return Host->OnResized();

	default:
		return DefWindowProc(
			hwnd, umsg,
			wparam,
			lparam
			);
	}
}

#pragma endregion

Framework::Application::Window *Framework::Application::Window::GetForCurrentThread(
	)
{
	static Framework::Application::Window window;
	return &window;
}

LRESULT Framework::Application::Window::OnMouseMoved(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	//__NENA_WINDOW_DbgPs "OnMouseMoved %i %i\n", x, y
		//__NENA_WINDOW_DbgPe
#endif

	Renderer.OnMouseMoved(::Nena::Vector2(x, y));

	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{

	Renderer.OnMouseLPressed(::Nena::Vector2(x, y));

	return 0;
}

LRESULT Framework::Application::Window::OnMouseRightButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
	Renderer.OnMouseRPressed(::Nena::Vector2(x, y));
	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonReleased(
	_In_::INT16 x, _In_::INT16 y
	)
{
	Renderer.OnMouseLReleased(::Nena::Vector2(x, y));
	return 0;
}

LRESULT Framework::Application::Window::OnMouseRightButtonReleased(
	_In_::INT16 x, _In_::INT16 y
	)
{
	Renderer.OnMouseRReleased(::Nena::Vector2(x, y));
	return 0;
}

LRESULT Framework::Application::Window::OnKeyPressed(
	_In_::UINT16 virtualKey
	)
{
	if (virtualKey == VK_ESCAPE)
	{

		if (Fullscreen)
		{
			::SendMessage(
				Raw, WM_CLOSE,
				0, 0
				);
		}
		else
		{
			::INT32 result = ::MessageBox(NULL,
				__TEXT("Are you sure you want to quit?"),
				__TEXT("Nena"), MB_YESNO
				);

			if (result == IDYES)
				::SendMessage(
				Raw, WM_CLOSE,
				0, 0
				);
		}
	}

	return 0;
}


LRESULT Framework::Application::Window::OnResized()
{
	Nena::Graphics::Resources::SizeF size;
	::BOOL status; ::RECT client = { 0 };
	status = ::GetClientRect(Raw, &client);
	size.Width = (float) client.right;
	size.Height = (float) client.bottom;

	Graphics.SetLogicalSize(size);
	Renderer.CreateWindowSizeDependentResources();
	return 0;
}

::BOOL Framework::Application::Window::Update()
{
	static ::HRESULT result = S_OK;

	//auto dx = (float) rand() / (float) RAND_MAX * 100.0f;
	//auto dy = (float) rand() / (float) RAND_MAX * 100.0f;

	//auto posx = Graphics.RenderTargetSize.Width * 0.5f + sinf(dx) * 10.0f;
	//auto posy = Graphics.RenderTargetSize.Height * 0.5f + cosf(dx) * 10.0f;

	/*posx *= 0.01f;
	posy *= 0.01f;*/

	//Renderer.OnMouseMoved({ posx, posy });

#if !__Nena_Framework_Discard_timer_updates

	static char string[16];
	sprintf_s(string, "F [%3u] \n",
		Timer.GetFramesPerSecond(),
		Timer.GetTotalSeconds());

	SetWindowTextA(Raw, string);

#endif

	Graphics.Clear(Graphics.Context.Get());

	Renderer.Update();
	Renderer.Render();

	Graphics.BindToPipeline(Graphics.Context.Get());
	result = Graphics.Present();
	Graphics.UnbindFromPipeline(Graphics.Context.Get());

	return TRUE;
}

Framework::Application::Window::Window(
	) : EventHandler(this), DeviceNotify(this), Graphics(
	Nena::Graphics::Resources::DriverType::D3D_DRIVER_TYPE_HARDWARE,
	//Nena::Graphics::Resources::DriverType::D3D_DRIVER_TYPE_REFERENCE,
	DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
	//DXGI_SWAP_EFFECT_DISCARD,
	DXGI_ALPHA_MODE_IGNORE,
	2, 0, 0, 1
	)
{
}

void Framework::Application::Start(
	)
{
	::HRESULT error;

	Window *window = Window::GetForCurrentThread();
	/*window->ScreenWidth = 400;
	window->ScreenHeight = 400;*/

	window->Style = Window::DefaultWindowStyle;
	//window->Style = Window::GameWindowStyle;
	window->Fullscreen = FALSE; // TRUE;

	MSG msg; ZeroMemory(&msg, sizeof MSG);
	window->Initialize(Loop); 
	 
	Window::Handle handle = window->Raw;

	HANDLE icoSmall = ::LoadImage(window->Instance, "../@Icons/fluids.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
	HANDLE icoBig = ::LoadImage(window->Instance, "../@Icons/fluids.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
	::SendMessage(handle, WM_SETICON, ICON_SMALL, (LPARAM) icoSmall);
	::SendMessage(handle, WM_SETICON, ICON_BIG, (LPARAM) icoBig);
	::SetWindowText(handle, __TEXT("Volume-Fluid.v.1.0.1.1"));

	window->Graphics.RegisterDeviceNotify(&window->DeviceNotify);
	window->Graphics.Windowed = !window->Fullscreen;
	window->Graphics.Vsync = TRUE;

	error = window->Graphics.SetWindow(handle);
	//error = window->Overlay.CreateDeviceResources(window->Graphics.Device.Get());

	if (SUCCEEDED(error))
	{
		/*window->Overlay.SetDpi(
			window->Graphics.Dpi.x, window->Graphics.Dpi.y
			);
		error = window->Overlay.CreateWindowSizeDependentResources(
			window->Graphics.Swapchain.Get()
			);*/
	}

	if (FAILED(error))
	{
		MessageBox(handle,
			__TEXT("Failed to create device/overlay resources.\nExiting application..."),
			__TEXT("Error"), MB_OK
			);
		return;
	}

	if (SUCCEEDED(error))
	{
		window->Renderer.Viewer = &window->Viewer;
		window->Renderer.D3DResources = &window->Graphics;

		/*window->Renderer.OnMouseLPressed(::Nena::Vector2(
			window->Graphics.RenderTargetSize.Width / 2.0f,
			window->Graphics.RenderTargetSize.Height / 2.0f
			));*/

		window->Renderer.SetImpactRadius(1);
		window->Renderer.SetVolumeSize(160);
		window->Renderer.CreateDeviceIndependentResources();
		window->Renderer.CreateDeviceDependentResources();
		window->Renderer.CreateWindowSizeDependentResources();
	}

	if (FAILED(error))
	{
		MessageBox(handle, 
			__TEXT("Failed to create renderer resources.\nExiting application..."),
			__TEXT("Error"), MB_OK
			);
		return;
	}

	::BOOL loop = TRUE;
	while (loop == TRUE)
	{
		if (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			OutputDebugStringA("WM_QUIT\n");
			loop = FALSE;
		}
		else 
		{
#if !__Nena_Framework_Discard_timer_updates
			window->Timer.Tick(
				[&window] { window->Update(); }
				);
#else
			window->Update();
#endif
		}
	}
}

void Framework::Application::Stop(
	)
{
	Framework::Application::Window::GetForCurrentThread()->OnClosed();
}