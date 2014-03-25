#include "Window.h"

Nena::Application::Window::Window() :
	ScreenWidth(800), ScreenHeight(600),
	Instance(::GetModuleHandle(NULL)),
	Style(GameWindowStyle),
	Name(__TEXT("NenaWindowClass")),
	Fullscreen(false),
	Raw(nullptr)
{
}

void Nena::Application::Window::Initialize(
	_In_ Nena::Application::Window::Callback proc,
	_In_opt_ Nena::Application::Window::Boolean cursor
	)
{
	::INT status = 0;
	Window::ScreenSettings screen;
	Window::WindowClass wc;
	Window::Int32 x, y;

	ZeroMemory(&screen, sizeof Window::ScreenSettings);
	ZeroMemory(&wc, sizeof Window::WindowClass);

	// Setup windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hbrBackground = (::HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.cbSize = sizeof(WindowClass);
	wc.lpszClassName = Name.c_str();
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = proc;
	wc.hIconSm = wc.hIcon;
	wc.hInstance = Instance;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;

	// Register window class.
	__NENA_WINDOW_Dbgvar(atom)
		::RegisterClassEx(&wc);

	if (Fullscreen)
	{
		ZeroMemory(&screen, sizeof ScreenSettings);
		status = EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &screen);
		ScreenWidth = (UInt16) screen.dmPelsWidth;
		ScreenHeight = (UInt16) screen.dmPelsHeight;

		/*
		ScreenWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
		ScreenHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);
		ScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		ScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
		*/
		x = y = 0;

		ZeroMemory(&screen, sizeof ScreenSettings);

		screen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		screen.dmSize = sizeof ScreenSettings;
		screen.dmPelsHeight = ScreenHeight;
		screen.dmPelsWidth = ScreenWidth;
		screen.dmBitsPerPel = 32;

		status = ::ChangeDisplaySettings(&screen, CDS_FULLSCREEN);
	}
	else
	{
		x = (::GetSystemMetrics(SM_CXFULLSCREEN) - ScreenWidth) / 2;
		y = (::GetSystemMetrics(SM_CYFULLSCREEN) - ScreenHeight) / 2;
		/*
		x = (::GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		y = (::GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;
		*/
	}

	Raw = ::CreateWindowEx(WS_EX_APPWINDOW,
		Name.c_str(), Name.c_str(), Style,
		x, y, ScreenWidth, ScreenHeight,
		nullptr, nullptr, Instance,
		nullptr
		);

	::ShowWindow(Raw, SW_SHOW);
	::SetForegroundWindow(Raw);
	::SetFocus(Raw);
	::ShowCursor(cursor);

}

BOOL Nena::Application::Window::EnablePointerEvents(
	_In_opt_ BOOL enable
	)
{
	BOOL result = FALSE;
	if (enable && (result = ::IsMouseInPointerEnabled())) return TRUE;
	if (!enable && !(result = ::IsMouseInPointerEnabled())) return TRUE;

	result = EnableMouseInPointer(enable);

#if __NENA_WINDOW_Debug__
	DWORD error;
	if (!result)
	{
		error = ::GetLastError();
		__NENA_WINDOW_DbgPs __NENA_WINDOW_LibNm
			" >> ErrorCode = %x", error
			__NENA_WINDOW_DbgPe
	}
#endif

	return result;
}

void Nena::Application::Window::OnClosed()
{
	::ShowCursor(true);
	if (Fullscreen) ::ChangeDisplaySettings(nullptr, 0);
	::DestroyWindow(Raw);
	::UnregisterClass(Name.c_str(), Instance);

	Instance = nullptr;
	Raw = nullptr;
}

void Nena::Application::Window::SetIcon(
	Window::String spath,
	Window::String bpath,
	UINT32 swpx, UINT32 shpx, 
	UINT32 bwpx, UINT32 bhpx
	)
{
	HANDLE sico = ::LoadImage(Instance, spath.c_str(), IMAGE_ICON, swpx, shpx, LR_LOADFROMFILE);
	HANDLE bico = ::LoadImage(Instance, bpath.c_str(), IMAGE_ICON, bwpx, bhpx, LR_LOADFROMFILE);
	if (sico) ::SendMessage(Raw, WM_SETICON, ICON_SMALL, (LPARAM) sico);
	if (bico) ::SendMessage(Raw, WM_SETICON, ICON_BIG, (LPARAM) bico);
}


void Nena::Application::Window::SetTitel(
	Window::String text
	)
{
	::SetWindowTextA(Raw, text.c_str());
}