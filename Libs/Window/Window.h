#include <Windows.h>
#include <Windowsx.h>
#include <string>
#include <sal.h>
#include <Nena\Delegate.h>

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
			typedef ::Nena::Delegate<void, Window *> WindowHandler;
			typedef ::Nena::Event<void, Window *> WindowEvent;
			typedef ::WNDCLASSEXA WindowClass;
			typedef ::DEVMODEA ScreenSettings;
			typedef ::HINSTANCE ModuleHandle;
			typedef ::std::string String;
			typedef ::WNDPROC Callback;
			typedef ::BOOL Boolean;
			typedef ::HWND Handle;
			typedef void *Unknown;

			typedef unsigned __int16 UInt16, UShort;
			typedef unsigned __int32 UInt32, UInt;
			typedef unsigned long DWord, ULong;
			typedef __int32 Int32, Int;
			typedef __int16 Int16;

			//! resizeable borderless window @note it is up to developer to handle sizing events
			static const ULong GameResizeableWindowStyle =
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_THICKFRAME | WS_VISIBLE;
			//! borderless window @see ToggleFullscreen
			static const ULong GameWindowStyle =
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_VISIBLE;
			//! ordinary window @note cannot be resized, but still can be moved is a usual way 
			//! using this style, developer does not need to handle size changed events like 
			//! @example WM_SIZE, WM_SIZING, WM_EXITSIZEMOVE
			static const ULong DefaultNonresizeableWindowStyle =
				WS_OVERLAPPEDWINDOW | WS_VISIBLE & ~WS_THICKFRAME;
			//! ordinary window @note it is up to developer to handle sizing events
			static const ULong DefaultWindowStyle =
				WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			/// Events

			Window::WindowEvent ResizedMoved;	//! should be emitted externally (by game engine)
			Window::WindowEvent SizeChanged;	//! should be emitted externally (by game engine)
			Window::WindowEvent Toggled;		//! fires with user toggles fullscreen/windowed modes
			Window::WindowEvent Created;		//! fires when window was successfully created
			Window::WindowEvent Closing;		//! fires just before this window will be closed
			Window::WindowEvent Closed;			//! fires after this window was closed

			/// Dimensions

			Window::UInt16 ScreenWidth, ScreenHeight;	//! current monitor dimensions (window dimensions in fullscreen, are always up-to-date)
			Window::UInt16 Width, Height;				//! window dimensions (when in windowed mode, are always up-to-date)
			Window::Int16 X, Y;							//! window position (are always up-to-date for both modes)

			/// Data

			Window::ModuleHandle Instance;	//! application instance @note do not modify
			Window::Boolean Fullscreen;		//! current window mode @note do not modify
			Window::Unknown UserData;		//! points to user data (if any, is null by default)
			Window::String Name;			//! this window title, @see SetTitle
			Window::Handle Raw;				//! this window handle, so the developer could use it @note do not modify
			Window::ULong Style;			//! current window style, @see static window styles
			Window::Boolean TrustDxgi;		//! when switching to/from fullscreen 

			Window();
			~Window();

			// Initializes window class, creates window.
			void Initialize(
				_In_ Window::Callback proc,
				_In_opt_ Window::Boolean cursor = true
				);

			// Turn on support on mouse input support in WM_POINTER.
			// If succeeds, returns non-zero, otherwise - zero.
			Boolean EnablePointerEvents(_In_opt_ BOOL enable = TRUE);

			void Close();

			// Unregisters window class, destroyes window, 
			// changes display settings back to normal.
			void OnClosed();

			// Sets window title (if default style is used, otherwise its useless)
			void SetTitle(
				_In_ Window::String text
				);

			// Sets icons for the window
			void SetIcon(
				_In_ Window::String spath, // path to small icon
				_In_ Window::String bpath, // path to big icon
				_In_opt_ UINT32 swpx = 64, _In_opt_ UINT32 shpx = 64, // small icon dims
				_In_opt_ UINT32 bwpx = 128, _In_opt_ UINT32 bhpx = 128 // big icon dims
				);

			// Toggles to fullscreen mode and back
			Window::Boolean ToggleFullscreen();

			void UpdateSizePosition();

			// Prints last error, closes and exits application
			static void TerminateProcess(
				const char *criticalLocation // function name
				);
		};
	}
}

#endif // !__NENA_WINDOW_H__
