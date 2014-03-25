#include "Framework.h"

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

Framework::Application::Window *Framework::Application::Window::GetForCurrentThread(
	)
{
	static Framework::Application::Window window;
	return &window;
}

Framework::Application::WindowDeviceNotify::WindowDeviceNotify(Framework::Application::Window *host) : Host(host)
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

Framework::Application::Window::Window(
	) : EventHandler(this), DeviceNotify(this), Graphics()
{
}

Framework::Application::Dispatcher::Dispatcher(
	Framework::Application::Window *host
	) : Host(host)
{
}

LRESULT Framework::Application::Window::OnMouseMoved(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	//__NENA_WINDOW_DbgPs "OnMouseMoved %i %i\n", x, y
		//__NENA_WINDOW_DbgPe
#endif

	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	__NENA_WINDOW_DbgPs ">>\telapsed time %f\n\ttotal %f\n\tframerate %u\n", 
		Timer.GetElapsedSeconds(),
		Timer.GetTotalSeconds(),
		Timer.GetFramesPerSecond()
		__NENA_WINDOW_DbgPe
#endif

	return 0;
}

LRESULT Framework::Application::Window::OnMouseRightButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonReleased(
	_In_::INT16 x, _In_::INT16 y
	)
{
	return 0;
}

LRESULT Framework::Application::Window::OnMouseRightButtonReleased(
	_In_::INT16 x, _In_::INT16 y
	)
{
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
	return 0;
}

::BOOL Framework::Application::Window::Update()
{
	static ::HRESULT result = S_OK;
	static ::FLOAT dirR = 2.0f;
	static ::FLOAT dirB = 2.0f;
	static ::FLOAT dirG = 2.0f;

	Graphics.ClearColor.R(Graphics.ClearColor.R() + dirR * 0.0011f);
	Graphics.ClearColor.G(Graphics.ClearColor.G() + dirG * 0.0033f);
	Graphics.ClearColor.B(Graphics.ClearColor.B() + dirB * 0.0055f);
	if (Graphics.ClearColor.R() >= 1.0f || Graphics.ClearColor.R() <= 0.0f) dirR *= -1.0f;
	if (Graphics.ClearColor.G() >= 1.0f || Graphics.ClearColor.G() <= 0.0f) dirG *= -1.0f;
	if (Graphics.ClearColor.B() >= 1.0f || Graphics.ClearColor.B() <= 0.0f) dirB *= -1.0f;

	Graphics.Clear(Graphics.Context.Get());

	Graphics.BindToPipeline(Graphics.Context.Get());
	result = Graphics.Present();
	Graphics.UnbindFromPipeline(Graphics.Context.Get());

	return TRUE;
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

#define __Icons_pathA "../@Icons/"
#define __Icons_pathW L"../@Icons/"

void Framework::Application::Start(
	)
{
	::HRESULT error;

	Window *window = Window::GetForCurrentThread();
	window->Style = Window::DefaultWindowStyle;
	window->Fullscreen = FALSE; // TRUE;

	MSG msg; ZeroMemory(&msg, sizeof MSG);
	window->Initialize(Loop);

	Window::Handle handle = window->Raw;

	HANDLE icoSmall = ::LoadImage(window->Instance, __Icons_pathA "cpp.ico", IMAGE_ICON, 64, 64, LR_LOADFROMFILE);
	HANDLE icoBig = ::LoadImage(window->Instance, __Icons_pathA "cpp.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
	if (icoSmall) ::SendMessage(handle, WM_SETICON, ICON_SMALL, (LPARAM) icoSmall);
	if (icoBig) ::SendMessage(handle, WM_SETICON, ICON_BIG, (LPARAM) icoBig);

	::SetWindowText(handle, __TEXT("DirectX Framework"));
	window->Graphics.RegisterDeviceNotify(&window->DeviceNotify);
	window->Graphics.Windowed = !window->Fullscreen;
	window->Graphics.Vsync = TRUE;

	error = window->Graphics.SetWindow(handle);
	if (FAILED(error))
	{
		MessageBox(handle, __TEXT("Failed to create device resources.\nExiting application..."),
			__TEXT("Error"), MB_OK);
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
			window->Timer.Tick(
				[&window] { window->Update(); }
				);
		}
	}
}

void Framework::Application::Stop(
	)
{
	Framework::Application::Window::GetForCurrentThread()->OnClosed();
}