#include "app.precompiled.h"

#ifndef __NENA_INTERACTIVE_TV_INPUT_INCLUDED__
#define __NENA_INTERACTIVE_TV_INPUT_INCLUDED__

namespace InteractiveTV
{
	struct Input
	{
		Nena::App::PaintEvent Paint;
		Nena::Dispatcher::KeyboardEvent KeyPressed;
		Nena::Dispatcher::KeyboardEvent KeyReleased;
		Nena::Dispatcher::MouseEvent MouseMoved;
		Nena::Dispatcher::MouseEvent MouseWheelChanged;
		Nena::Dispatcher::MouseEvent MouseHWheelChanged;
		Nena::Dispatcher::MouseEvent MouseLeftButtonPressed;
		Nena::Dispatcher::MouseEvent MouseLeftButtonReleased;
		Nena::Dispatcher::MouseEvent MouseRightButtonPressed;
		Nena::Dispatcher::MouseEvent MouseRightButtonReleased;
		Nena::Dispatcher::MouseEvent MouseMiddleButtonPressed;
		Nena::Dispatcher::MouseEvent MouseMiddleButtonReleased;

		static LRESULT _Nena_DispatcherCallTy_ KeyboardDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);
		static LRESULT _Nena_DispatcherCallTy_ MouseDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);
		static LRESULT _Nena_DispatcherCallTy_ MouseWheelDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);
		static LRESULT _Nena_DispatcherCallTy_ MouseHWheelDispatchCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);
		static LRESULT _Nena_DispatcherCallTy_ PaintCallback(
			_In_ Nena::Dispatcher::Event *e,
			_In_ HWND hwnd, _In_ UINT32 msg,
			_In_ WPARAM wparam, _In_ LPARAM lparam
			);

		Input();
		void Init();

	private:

		::BOOL m_app_pipeline_configured;
	};
}

#endif // !__NENA_INTERACTIVE_TV_INPUT_INCLUDED__
