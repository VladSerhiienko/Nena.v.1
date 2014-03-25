#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include <sal.h>

#if _DEBUG || DEBUG
#define __NENA_WINDOW_Debug__ 1
#include <debugapi.h>
#endif

#if NTDDI_VERSION == NTDDI_WINBLUE
#define __NENA_WINDOW_WindowsBlue__ 1
#elif NTDDI_VERSION == NTDDI_WIN8
#define __NENA_WINDOW_Windows8__ 1
#elif NTDDI_VERSION == NTDDI_WIN7
#define __NENA_WINDOW_Windows7__ 1
#else
#warning "Unsupported Windows Version"
#endif

#if __NENA_WINDOW_WindowsBlue__ || __NENA_WINDOW_Windows8__
#include <WinUser.h>
#endif

#ifndef __NENA_WINDOW_H__
#define __NENA_WINDOW_H__


namespace Nena
{

#if __NENA_WINDOW_Debug__ // Debugging

	namespace Application
	{
		static char DbgMsg[512] = { 0 };
	}

#define __NENA_WINDOW_LibNm "Nena::Window"
#define __NENA_WINDOW_DbgPs { ::sprintf_s(Nena::Application::DbgMsg, 
#define __NENA_WINDOW_DbgPe ); OutputDebugStringA(Nena::Application::DbgMsg); }
#define __NENA_WINDOW_Dbgvar(var) auto var =

#else // Releasing

#define __NENA_WINDOW_Dbgvar(var)

#endif // __NENA_WINDOW_Debug__

	namespace Application
	{
		struct Window
		{
			typedef unsigned __int16 UInt16, UShort;
			typedef unsigned __int32 UInt32, UInt;
			typedef unsigned long DWord, ULong;
			typedef __int32 Int32, Int;
			typedef void *Unknown;
			typedef bool Boolean;

			typedef ::WNDCLASSEX WindowClass;
			typedef ::DEVMODE ScreenSettings;
			typedef ::HINSTANCE ModuleHandle;
			typedef ::WNDPROC Callback;
			typedef std::string String;
			typedef ::HWND Handle;

			static const ULong GameWindowStyle = 
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
			static const ULong DefaultWindowStyle =
				WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			UInt16 ScreenWidth, ScreenHeight;
			Window::ModuleHandle Instance;
			Window::Boolean Fullscreen;
			Window::Unknown UserData;
			Window::String Name;
			Window::Handle Raw;
			Window::ULong Style;

			Window();

			// Initializes window class, creates window.
			void Initialize(
				_In_ Window::Callback proc,
				_In_opt_ Window::Boolean cursor = true
				);

			// Turn on support on mouse input support in WM_POINTER.
			// If succeeds, returns non-zero, otherwise - zero.
			// Use ::GetLastError() for extended information.
			BOOL EnablePointerEvents(_In_opt_ BOOL enable = TRUE);

			// Unregisters window class, destroyes window, 
			// changes display settings back to normal.
			void OnClosed();

			void SetTitel(
				Window::String text
				);

			void SetIcon(
				Window::String spath,
				Window::String npath,
				UINT32 swpx = 64, UINT32 shpx = 64, 
				UINT32 bwpx = 128, UINT32 bhpx = 128
				);
		};
	}
}

#endif // !__NENA_WINDOW_H__
