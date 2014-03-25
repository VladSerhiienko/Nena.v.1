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

Framework::Application::Window::Window(
	) : EventHandler(this)
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
	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	__NENA_WINDOW_DbgPs "OnMouseLeftButtonPressed %i %i\n", x, y
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

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

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
	//::HRESULT error;

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


	::BOOL status = ::SetWindowText(handle, __TEXT("Framework"));

	while (true)
	{
		if (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;
	}
}

void Framework::Application::Stop(
	)
{
	Framework::Application::Window::GetForCurrentThread()->OnClosed();
}