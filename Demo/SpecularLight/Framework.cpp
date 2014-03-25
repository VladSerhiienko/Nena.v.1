#include "Framework.h"
#include "ContentLoader.h"

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
	case WM_MOUSEWHEEL:
		return Host->OnMouseWheel(
			GET_WHEEL_DELTA_WPARAM(wparam)
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

Framework::Application::Window::Window(
	) : Graphics(Nena::Graphics::Resources::DriverType::D3D_DRIVER_TYPE_HARDWARE),
	//) : Graphics(Nena::Graphics::Resources::DriverType::D3D_DRIVER_TYPE_REFERENCE),
	EventHandler(this), DeviceNotify(this), Overlay()//, SceneContent(this)
{
}

LRESULT Framework::Application::Window::OnMouseWheel(::INT32 delta)
{
	if (RMButtonPressed) // && 0)
	{
		Viewer.UpdateView({ 0, 0 }, (::FLOAT) delta);
	}

	return 0;
}

LRESULT Framework::Application::Window::OnMouseMoved(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	//__NENA_WINDOW_DbgPs "OnMouseMoved %i %i\n", x, y
		//__NENA_WINDOW_DbgPe
#endif

	static ::Nena::Vector2 previousPos = { (::FLOAT) x, (::FLOAT) y };
	static ::Nena::Vector2 delta = { (::FLOAT) 0, (::FLOAT) 0 };

	if (RMButtonPressed) // && 0)
	{
		delta.x = x - previousPos.x;
		delta.y = y - previousPos.y;
		previousPos.x = x;
		previousPos.y = y;
		Viewer.UpdateView(delta, 0);
	}
	else
	{
		previousPos.x = x;
		previousPos.y = y;
	}

	return 0;
}

LRESULT Framework::Application::Window::OnMouseLeftButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
#if __NENA_WINDOW_Debug__
	__NENA_WINDOW_DbgPs 
		">>\telapsed time %f\n"
		">>\tframerate %u\n"
		">>\ttotal %f\n"
		"---------------------------\n",
		Timer.GetElapsedSeconds(),
		Timer.GetFramesPerSecond(),
		Timer.GetTotalSeconds()
		__NENA_WINDOW_DbgPe
#endif

	return 0;
}

LRESULT Framework::Application::Window::OnMouseRightButtonPressed(
	_In_::INT16 x, _In_::INT16 y
	)
{
	RMButtonPressed = TRUE;
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
	RMButtonPressed = FALSE;
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

	Viewer.AspectRatio = size.Width / size.Height;
	Viewer.UpdateProjection();

	Graphics.SetLogicalSize(size);
	return 0;
}

::BOOL Framework::Application::Window::Update()
{
	static ::HRESULT result = S_OK;

	Graphics.Clear(Graphics.Context.Get());
	Graphics.BindToPipeline(Graphics.Context.Get());

	if (RMButtonPressed)
	{
		//OutputDebugStringA("Renderer.Update()\n");
		Renderer.Update();
	}

	Renderer.Render();

	result = Graphics.Present();
	Graphics.UnbindFromPipeline(Graphics.Context.Get());

	return TRUE;
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

	::SetWindowText(handle, __TEXT("Specular Light"));
	window->Graphics.RegisterDeviceNotify(&window->DeviceNotify);
	window->Graphics.Windowed = !window->Fullscreen;
	window->Graphics.Vsync = TRUE;

	error = window->Graphics.SetWindow(handle);

	if (SUCCEEDED(error))
	{

		///*
		window->Renderer.ShapeTexturePath = L"D:/Development/VisualStudio/Aktuelle/Nena/Bin/@Resources/Image004.jpg";
		//window->Renderer.ShapeTexturePath = L"D:/Development/VisualStudio/Aktuelle/Nena/Bin/@Resources/earth-colors.dds";
		//window->Renderer.ShapeTexturePath = L"D:/Development/VisualStudio/Aktuelle/Nena/Bin/@Resources/earth-normals.dds";
		window->Loader.UploadShape("D:/Development/Maya/Scenes/sphere-2-20x20-t.ma.xml", "pSphereShape1", "pSphere1", &window->Content);
		window->Content.TransformMWS = ::Nena::XMMatrixRotationRollPitchYaw(
			::Nena::XM_PI, 0, 0
			);
		//*/

		//window->Renderer.ShapeTexturePath = L"D:/Development/VisualStudio/Aktuelle/Nena/Bin/@Resources/Image004.jpg";
		//window->Loader.UploadShape("D:/Development/Maya/Scenes/cube-2x2x2-t.ma.xml", "pCubeShape1", "pCube1", &window->Content);

		window->Diffuse.AmbientColor = (Nena::XMVECTOR) ::Nena::Colors::WhiteSmoke;
		window->Diffuse.DiffuseColor = (Nena::XMVECTOR) ::Nena::Colors::White;
		window->Diffuse.SpecularColor = (Nena::XMVECTOR) ::Nena::Colors::Yellow;
		window->Diffuse.SpecularPower = 32;
		
		window->Diffuse.DiffuseDirection.y = -1;
		window->Diffuse.DiffuseDirection.x = -1;

		window->Renderer.D3DResources = &window->Graphics;
		window->Renderer.D2DResources = &window->Overlay;
		window->Renderer.Vbdata = &window->Content.PolygonList;
		window->Renderer.Pbdata = &window->Content.PointListOS;
		window->Renderer.Nbdata = &window->Content.NormalListOS;
		window->Renderer.Tbdata = &window->Content.TexcoordsList;
		window->Renderer.ShapeW = &window->Content.TransformMWS;
		window->Renderer.Camera = &window->Viewer;
		window->Renderer.Light = &window->Diffuse;

		window->Renderer.CreateDeviceDependentResources();

	}

	if (FAILED(error))
	{
		MessageBox(handle, 
			__TEXT("Failed to create device/overlay resources.\nExiting application..."),
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

		if (msg.message == WM_QUIT) loop = FALSE; else 
			window->Timer.Tick([&window] { window->Update(); });
	}
}

void Framework::Application::Stop(
	)
{
	Framework::Application::Window::GetForCurrentThread()->OnClosed();
}