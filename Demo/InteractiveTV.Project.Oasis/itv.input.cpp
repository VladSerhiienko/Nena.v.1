#include "app.precompiled.h"
#include "itv.oasis.h"
#include "itv.remote.control.h"
#include "itv.input.h"

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::PaintCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static auto oasis = InteractiveTV::Project::Oasis::GetForCurrentThread( );

	if ( e->Msg == msg )
	{
		oasis->OnFrameMove( );
		oasis->Context.Timer.Update( );
		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::KeyboardDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static auto oasis = InteractiveTV::Project::Oasis::GetForCurrentThread( );

	if ( e->Msg == msg )
	{
		if ( msg == Nena::Application::Message::KeyPressed )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnKeyPressed( args.u32[ 0 ] );
		}
		else if ( msg == Nena::Application::Message::KeyReleased )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			if ( args.u32[ 0 ] == Nena::Application::VirtualKey::F )
				oasis->OnToggleFullscreen( );
			else if ( args.u32[ 0 ] == Nena::Application::VirtualKey::Escape )
				oasis->Quit( );
			else
				oasis->OnKeyReleased( args.u32[ 0 ] );
		}

		return 0;
	}
	else return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::MouseDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static auto oasis = InteractiveTV::Project::Oasis::GetForCurrentThread( );

	if ( e->Msg == msg )
	{
		if ( msg == Nena::Application::Message::MouseMove )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnMouseMoved( args.f32[ 0 ], args.f32[ 1 ] );
		}
		else if ( msg == Nena::Application::Message::LeftButtonPressed )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnMouseLBPressed( args.f32[ 0 ], args.f32[ 1 ] );
		}
		else if ( msg == Nena::Application::Message::LeftButtonReleased )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnMouseLBReleased( args.f32[ 0 ], args.f32[ 1 ] );
		}
		else if ( msg == Nena::Application::Message::RightButtonPressed )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnMouseRBPressed( (FLOAT) args.u32[ 0 ], (FLOAT) args.u32[ 1 ] );
		}
		else if ( msg == Nena::Application::Message::RightButtonReleased )
		{
			auto args = e->Parser( e, hwnd, msg, wparam, lparam );
			oasis->OnMouseRBReleased( args.f32[ 0 ], args.f32[ 1 ] );
		}

		return 0;
	}
	return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::SizeChangedCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static auto oasis = InteractiveTV::Project::Oasis::GetForCurrentThread( );

	if ( e->Msg == msg )
	{
		if ( wparam != SIZE_MINIMIZED )
		{
			auto w = LOWORD( lparam );
			auto h = HIWORD( lparam );
			oasis->OnSizeChanged( w, h );
			return 0;
		}
	}

	return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::GestureReceivedCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	static auto oasis = InteractiveTV::Project::Oasis::GetForCurrentThread( );

	if ( e->Msg == msg )
	{
		if ( (UINT32) wparam > 90u )
		{
			oasis->OnGestureReceived( lparam );
			return 0;
		}
	}

	return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::MouseWheelDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

LRESULT _Nena_DispatcherCallTy_ InteractiveTV::Input::MouseHWheelDispatchCallback(
	_In_ Nena::Dispatcher::Event *e,
	_In_ HWND hwnd, _In_ UINT32 msg,
	_In_ WPARAM wparam, _In_ LPARAM lparam
	)
{
	return e->OnMismatch->Dispatch(
		hwnd, msg, wparam, lparam
		);
}

InteractiveTV::Input::Input(
	)
	: KeyPressed( Nena::Application::Message::KeyPressed )
	, KeyReleased( Nena::Application::Message::KeyReleased )
	, MouseMoved( Nena::Application::Message::MouseMove )
	, MouseWheelChanged( Nena::Application::Message::MouseWheel )
	, MouseHWheelChanged( Nena::Application::Message::MouseHWheel )
	, MouseLeftButtonPressed( Nena::Application::Message::LeftButtonPressed )
	, MouseLeftButtonReleased( Nena::Application::Message::LeftButtonReleased )
	, MouseRightButtonPressed( Nena::Application::Message::RightButtonPressed )
	, MouseRightButtonReleased( Nena::Application::Message::RightButtonReleased )
	, MouseMiddleButtonPressed( Nena::Application::Message::MiddleButtonPressed )
	, MouseMiddleButtonReleased( Nena::Application::Message::MiddleButtonReleased )
	, m_app_pipeline_configured( false )
{
	MouseMoved.SetPriority( 45 ); // default is 50
	MouseWheelChanged.SetPriority( 55 ); // default is 50
	MouseHWheelChanged.SetPriority( 55 ); // default is 50

	GestureReceived.SetPriority( KeyPressed.GetPriority( ) );
	GestureReceived.Msg = InteractiveTV::Remote::Input::GestureAppMessage;

	Paint.OnDispatch = &PaintCallback;
	GestureReceived.OnDispatch = &GestureReceivedCallback;
	KeyPressed.OnDispatch = &KeyboardDispatchCallback;
	KeyReleased.OnDispatch = &KeyboardDispatchCallback;
	MouseMoved.OnDispatch = &MouseDispatchCallback;
	MouseWheelChanged.OnDispatch = &MouseWheelDispatchCallback;
	MouseHWheelChanged.OnDispatch = &MouseHWheelDispatchCallback;
	MouseLeftButtonPressed.OnDispatch = &MouseDispatchCallback;
	MouseLeftButtonReleased.OnDispatch = &MouseDispatchCallback;
	MouseRightButtonPressed.OnDispatch = &MouseDispatchCallback;
	MouseRightButtonReleased.OnDispatch = &MouseDispatchCallback;
	MouseMiddleButtonPressed.OnDispatch = &MouseDispatchCallback;
	MouseMiddleButtonReleased.OnDispatch = &MouseDispatchCallback;

	auto xcontext = &InteractiveTV::Project::Oasis::GetForCurrentThread( )->Context;
	xcontext->App->ViewSizeChanged.OnDispatch = &SizeChangedCallback;
}

void InteractiveTV::Input::Init( )
{
	if ( m_app_pipeline_configured ) return;

	Nena::App::GetForCurrentThread( )->Handler.Register( &Paint );

	Nena::App::GetForCurrentThread( )->Handler.Register( &KeyPressed );
	Nena::App::GetForCurrentThread( )->Handler.Register( &KeyReleased );
	Nena::App::GetForCurrentThread( )->Handler.Register( &GestureReceived );

	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseMoved );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseWheelChanged );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseHWheelChanged );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseLeftButtonPressed );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseLeftButtonReleased );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseRightButtonPressed );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseRightButtonReleased );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseMiddleButtonPressed );
	Nena::App::GetForCurrentThread( )->Handler.Register( &MouseMiddleButtonReleased );

	m_app_pipeline_configured = true;
}
