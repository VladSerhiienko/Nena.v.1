#include "Window.h"
#include <strsafe.h>
#include <iostream>

Nena::Application::Window::Window() 
	: ScreenWidth(800)
	, ScreenHeight(600)
	, Width(800)
	, Height(600)
	, Instance(::GetModuleHandle(NULL))
	, Style(GameWindowStyle)
	, Name(__TEXT("NenaWindowClass"))
	, Fullscreen(false)
	, Raw(nullptr)
{
	::OutputDebugStringA("Nena::Application::Window::Window()\n");
}

Nena::Application::Window::~Window()
{
	::OutputDebugStringA("Nena::Application::Window::~Window()\n");
	OnClosed();
}

void Nena::Application::Window::Close()
{
	::OutputDebugStringA("Nena::Application::Window::Close()\n");
	if (Raw) ::SendMessage(Raw, WM_CLOSE, NULL, NULL);
}

void Nena::Application::Window::Initialize(
	_In_ Nena::Application::Window::Callback proc,
	_In_opt_ Nena::Application::Window::Boolean cursor
	)
{
	if (Raw) return;

	::OutputDebugStringA("Nena::Application::Window::Initialize()\n");

	Window::WindowClass wc;
	Window::ScreenSettings screen;

	::INT status = 0;
	::HMONITOR monitorHandle;
	::MONITORINFOEXA monitorInfo;

	status = FALSE;
	monitorHandle = NULL;
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
	auto atom = ::RegisterClassExA(&wc);

	if (atom == INVALID_ATOM)
	{
		auto error = GetLastError();
		return;
	}
	else status = TRUE;

	status = TRUE;
	POINT point = { 0 };

	// in the virtual screen space, primary display rect starts from (0, 0),
	// so it should return a handle to a default device
	monitorHandle = ::MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY);
	if (monitorHandle == INVALID_HANDLE_VALUE || monitorHandle == NULL)
		OutputDebugStringA("\tFailed to get current monitor handle\n"),
		status = FALSE;
	ZeroMemory(&monitorInfo, sizeof MONITORINFOEXA);
	monitorInfo.cbSize = sizeof MONITORINFOEXA;
	if (status && !::GetMonitorInfoA(monitorHandle, &monitorInfo))
		OutputDebugStringA("\tFailed to get current monitor info\n"),
		status = FALSE;

	if (!status)
	{
		// an attempt to find monitor manually
		// or return the last one if ok
		status = EnumDisplayMonitors(NULL, NULL,
			(MONITORENUMPROC)[](
				HMONITOR monitor, HDC context,
				LPRECT rect, LPARAM data
				)
			{
				_CRT_UNUSED(context);
				((HMONITOR &) data) = monitor;
				if (rect->left == 0 && rect->top == 0) return FALSE;
				else return TRUE;
			},
			(LPARAM) (HMONITOR &) monitorHandle
			);

		if (status) status =
			monitorHandle != INVALID_HANDLE_VALUE &&
			monitorHandle != NULL;
	}

	if (status)
	{
		// current monitor was found
		ScreenWidth = (decltype(ScreenWidth)) (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
		ScreenHeight = (decltype(ScreenHeight)) (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
		X = (Int16) (monitorInfo.rcMonitor.left + (ScreenWidth - Width) / 2);
		Y = (Int16) (monitorInfo.rcMonitor.top + (ScreenHeight - Height) / 2);
	}
	else // all the attempts to find monitor device failed, try another approach
	{
		::ZeroMemory(&screen, sizeof ScreenSettings);
		if (!::EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &screen))
			OutputDebugStringA("\tFailed to get current display device\n"),
			status = FALSE,
			ScreenWidth = 0,
			ScreenHeight = 0;
		else
			ScreenWidth = (UInt16) screen.dmPelsWidth,
			ScreenHeight = (UInt16) screen.dmPelsHeight;

		X = Y = 0;
	}

	if (Fullscreen) Raw = ::CreateWindowExA(
		WS_EX_APPWINDOW, Name.c_str(), Name.c_str(), Style,
		X, Y, ScreenWidth, ScreenHeight, nullptr, nullptr, Instance,
		nullptr
		);
	else Raw = ::CreateWindowExA(
		WS_EX_APPWINDOW, Name.c_str(), Name.c_str(), Style,
		X, Y, Width, Height, nullptr, nullptr, Instance,
		nullptr
		);

	if (Raw) // if window was created
	{
		::ShowWindow(Raw, SW_SHOW);
		::SetForegroundWindow(Raw);
		::SetFocus(Raw);
		::ShowCursor(cursor);

		// adjust window
		Fullscreen = !Fullscreen;
		ToggleFullscreen();
	}

	// notify a developer about window was created 
	// (or was not, should check a handle)
	Created(this);
}

Nena::Application::Window::Boolean Nena::Application::Window::EnablePointerEvents(
	_In_opt_ BOOL enable
	)
{
	BOOL result = FALSE;
	if (enable && (result = ::IsMouseInPointerEnabled())) return TRUE;
	if (!enable && !(result = ::IsMouseInPointerEnabled())) return TRUE;

	result = ::EnableMouseInPointer(enable);

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
	if (!Raw) return;
	::OutputDebugStringA("Nena::Application::Window::OnClosed()\n");

	Closing(this);

	::ShowCursor(true);
	if (Fullscreen) ::ChangeDisplaySettings(nullptr, 0);
	::DestroyWindow(Raw);
	::UnregisterClass(Name.c_str(), Instance);

	Instance = nullptr;
	Raw = nullptr;

	Closed(this);
}

void Nena::Application::Window::SetIcon(
	Window::String spath,
	Window::String bpath,
	UINT32 swpx, UINT32 shpx, 
	UINT32 bwpx, UINT32 bhpx
	)
{
	::OutputDebugStringA("Nena::Application::Window::SetIcon()\n");
	HANDLE sico = ::LoadImage(Instance, spath.c_str(), IMAGE_ICON, swpx, shpx, LR_LOADFROMFILE);
	HANDLE bico = ::LoadImage(Instance, bpath.c_str(), IMAGE_ICON, bwpx, bhpx, LR_LOADFROMFILE);
	if (sico) ::SendMessage(Raw, WM_SETICON, ICON_SMALL, (LPARAM) sico);
	if (bico) ::SendMessage(Raw, WM_SETICON, ICON_BIG, (LPARAM) bico);
}

void Nena::Application::Window::SetTitle(
	Window::String text
	)
{
	::OutputDebugStringA("Nena::Application::Window::SetTitle()\n");
	::SetWindowTextA(Raw, text.c_str());
}

void Nena::Application::Window::UpdateSizePosition()
{
	::BOOL status;
	::HMONITOR monitorHandle;
	::MONITORINFOEXA monitorInfo;

	::OutputDebugStringA("Nena::Application::Window::UpdateSizePosition()\n");

	ZeroMemory(&monitorInfo, sizeof MONITORINFOEXA);
	monitorInfo.cbSize = sizeof MONITORINFOEXA;

	monitorHandle = ::MonitorFromWindow(Raw, MONITOR_DEFAULTTONEAREST);
	if (monitorHandle == INVALID_HANDLE_VALUE || monitorHandle == NULL)
	{
		::OutputDebugStringA("\tFailed to get current monitor handle\n");
		return;
	}
	if (!::GetMonitorInfoA(monitorHandle, &monitorInfo))
	{
		::OutputDebugStringA("\tFailed to get current monitor info\n");
		return;
	}

	//Style &= ~WS_THICKFRAME;
	ScreenWidth = (decltype(ScreenWidth)) (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
	ScreenHeight = (decltype(ScreenHeight)) (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);

	if (Fullscreen)
	{
		::RECT clientRect = { 0 };
		status = ::GetClientRect(Raw, &clientRect);
		Width = (decltype(Width)) clientRect.right;
		Height = (decltype(Height)) clientRect.bottom;

		Window::ScreenSettings screen;
		ZeroMemory(&screen, sizeof ScreenSettings);
		memcpy(&screen.dmDeviceName, monitorInfo.szDevice,
			min(sizeof screen.dmDeviceName, sizeof monitorInfo.szDevice));
		screen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		screen.dmSize = sizeof ScreenSettings;
		screen.dmPelsHeight = ScreenHeight;
		screen.dmPelsWidth = ScreenWidth;
		screen.dmBitsPerPel = 32;

		X = (Int16) (monitorInfo.rcMonitor.left);
		Y = (Int16) (monitorInfo.rcMonitor.top);
	}
	else
	{
		RECT rect;
		rect.right = Width;
		rect.bottom = Height;
		rect.top = monitorInfo.rcMonitor.top;
		rect.left = monitorInfo.rcMonitor.left;

		X = (Int16) ((ScreenWidth - Width) / 2 + monitorInfo.rcMonitor.left);
		Y = (Int16) ((ScreenHeight - Height) / 2 + monitorInfo.rcMonitor.top);
	}
}

Nena::Application::Window::Boolean Nena::Application::Window::ToggleFullscreen()
{
	::BOOL status;
	::HMONITOR monitorHandle;
	::MONITORINFOEXA monitorInfo;

	::OutputDebugStringA("Nena::Application::Window::ToggleFullscreen()\n");

	ZeroMemory(&monitorInfo, sizeof MONITORINFOEXA);
	monitorInfo.cbSize = sizeof MONITORINFOEXA;

	monitorHandle = ::MonitorFromWindow(Raw, MONITOR_DEFAULTTONEAREST);
	if (monitorHandle == INVALID_HANDLE_VALUE || monitorHandle == NULL)
	{
		::OutputDebugStringA("\tFailed to get current monitor handle\n");
		return FALSE;
	}
	if (!::GetMonitorInfoA(monitorHandle, &monitorInfo))
	{
		::OutputDebugStringA("\tFailed to get current monitor info\n");
		return FALSE;
	}

	//Style &= ~WS_THICKFRAME;
	ScreenWidth = (decltype(ScreenWidth)) (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);
	ScreenHeight = (decltype(ScreenHeight)) (monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);

	if (!Fullscreen)
	{
		::RECT clientRect = { 0 };
		status = ::GetClientRect(Raw, &clientRect);
		Width = (decltype(Width)) clientRect.right;
		Height = (decltype(Height)) clientRect.bottom;

		Window::ScreenSettings screen;
		ZeroMemory(&screen, sizeof ScreenSettings);
		memcpy(&screen.dmDeviceName, monitorInfo.szDevice,
			min(sizeof screen.dmDeviceName, sizeof monitorInfo.szDevice));
		screen.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		screen.dmSize = sizeof ScreenSettings;
		screen.dmPelsHeight = ScreenHeight;
		screen.dmPelsWidth = ScreenWidth;
		screen.dmBitsPerPel = 32;

		X = (Int16) (monitorInfo.rcMonitor.left);
		Y = (Int16) (monitorInfo.rcMonitor.top);

		::SetWindowLongPtrA(Raw, GWL_STYLE, GameWindowStyle);
		status = ::ChangeDisplaySettingsA(&screen, CDS_FULLSCREEN);
		status = ::SetWindowPos(Raw, HWND_TOPMOST, X, Y, ScreenWidth, ScreenHeight, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		status = ::PostMessageA(Raw, WM_EXITSIZEMOVE, 0, 0);

		Fullscreen = TRUE;
		Toggled(this);
	}
	else
	{
		RECT rect;
		rect.right = Width;
		rect.bottom = Height;
		rect.top = monitorInfo.rcMonitor.top;
		rect.left = monitorInfo.rcMonitor.left;

		::SetWindowLongPtrA(Raw, GWL_STYLE, Style);
		::AdjustWindowRect(&rect, Style, FALSE);

		X = (Int16) ((ScreenWidth - Width) / 2 + monitorInfo.rcMonitor.left);
		Y = (Int16) ((ScreenHeight - Height) / 2 + monitorInfo.rcMonitor.top);

		status = ::ChangeDisplaySettings(nullptr, 0);
		status = ::SetWindowPos(Raw, HWND_TOP, X, Y, Width, Height, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
		status = ::PostMessageA(Raw, WM_EXITSIZEMOVE, 0, 0);

		Fullscreen = FALSE;
		Toggled(this);
	}

	return true;
}

//! @see http://msdn.microsoft.com/en-us/library/ms680582%28VS.85%29.aspx 
void Nena::Application::Window::TerminateProcess(const char *criticalPlace)
{
	::OutputDebugStringA("Nena::Application::Window::TerminateProcess()\n");

	LPVOID messageBuffer;
	LPVOID displayMessageBuffer;
	auto lastError = GetLastError();

	::FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char *) &messageBuffer, 0, NULL
		);

	auto sz = (::lstrlenA((const char *) messageBuffer) + ::lstrlenA((const char *) criticalPlace) + 40) * sizeof(char);
	displayMessageBuffer = (LPVOID) ::LocalAlloc(LMEM_ZEROINIT, sz);

	auto result = ::StringCchPrintfA(
		(char *) displayMessageBuffer,
		::LocalSize(displayMessageBuffer) / sizeof(char),
		"%s failed with error %d: %s",
		criticalPlace, lastError, messageBuffer
		);

#if _DEBUG
	::OutputDebugStringA((const char *) displayMessageBuffer);
#else
	//if (Raw && Fullscreen) ToggleFullscreen();
	if (SUCCEEDED(result) && (::GetConsoleWindow() || ::AllocConsole()))
		::std::cout << (char *) displayMessageBuffer << ::std::endl,
		::std::cout << "Press any key to exit ..." << ::std::endl,
		::getchar();
#endif

	::LocalFree(messageBuffer);
	::LocalFree(displayMessageBuffer);
	::PostQuitMessage(lastError);
	::ExitProcess(lastError);
}

/// @todo add option to find the largest screen, or just use primary