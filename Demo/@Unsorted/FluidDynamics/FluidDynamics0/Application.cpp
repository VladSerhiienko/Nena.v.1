#include "Precompiled.h"
#include "Application.h"

void Sandbox::View::Load( _In_ Sandbox::View::EntryPointString wszEntryPoint )
{
}

void Sandbox::View::Uninitialize( )
{
#ifdef REIGN_DESKTOP
	Safe_Delete( m_renderer );
#endif
}

void Sandbox::View::Initialize( _In_ Sandbox::AppViewPtr view )
{
	m_bActivated = false;
	m_bClosed = false;
	m_bVisible = false;

#pragma warning(disable: 4316)
	m_renderer = alloc_ref_new Renderer( );
#pragma warning(default: 4316)

	m_renderer->Driver( D3D_DRIVER_TYPE_HARDWARE );
	//m_renderer->Driver( D3D_DRIVER_TYPE_REFERENCE );
}

void Sandbox::View::SetWindow( _In_ Sandbox::AppWindowPtr window )
{
	window->VisibilityChanged += alloc_ref_new Sandbox::View::AppWindowVisiblityEventHandler( this, &Sandbox::View::OnWindowVisibilityChanged );
	window->SizeChanged += alloc_ref_new Sandbox::View::AppWindowSizingEventHandler( this, &Sandbox::View::OnWindowSizeChanged );
	window->Activated += alloc_ref_new Sandbox::View::AppWindowActivationEventHandler( this, &Sandbox::View::OnWindowActivatedChanged );
	window->Closed += alloc_ref_new Sandbox::View::AppWindowEventHandler( this, &Sandbox::View::OnWindowClosed );

	window->PointerReleased += alloc_ref_new Sandbox::View::AppWindowPointerEventHandler( this, &Sandbox::View::OnPointerReleased );
	window->PointerPressed += alloc_ref_new Sandbox::View::AppWindowPointerEventHandler( this, &Sandbox::View::OnPointerPressed );
	window->PointerMoved += alloc_ref_new Sandbox::View::AppWindowPointerEventHandler( this, &Sandbox::View::OnPointerMoved );

	//m_renderer->Driver(D3D_DRIVER_TYPE_HARDWARE);
	//m_renderer->Window(window->GetHandle());
	m_renderer->Initialize( );

}

void Sandbox::View::Run( )
{
	auto window = Sandbox::AppWindow::GetForCurrentThread( );
	Sehnsucht::BasicTimer timer;

	window->Activate( );
	timer.Reset( );

	while ( !m_bClosed )
	{
		window->Dispatcher->ProcessEvents( AppWindowProcessEventsOption::ProcessAllIfPresent );
		m_renderer->OnFrameMove( &timer );
		m_renderer->OnFrame( );

		timer.Update( );
	}
}

void Sandbox::View::OnPointerMoved(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}

void Sandbox::View::OnPointerPressed(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}

void Sandbox::View::OnPointerReleased(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}

void Sandbox::View::OnWindowSizeChanged(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowSizingEventArgsPtr pArgs
	)
{
	//m_renderer->UpdateForWindowSizeChange( );
}

void Sandbox::View::OnWindowActivatedChanged(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowActivationEventArgsPtr pArgs
	)
{
	m_bActivated = pArgs->WindowActivationState != AppWindowActivationState::Deactivated;
}

void Sandbox::View::OnWindowVisibilityChanged(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowVisibilityEventArgsPtr pArgs
	)
{
	m_bVisible = pArgs->Visible;
}

void Sandbox::View::OnWindowClosed(
	_In_ Sandbox::AppWindowPtr pWindow,
	_In_ Sandbox::View::AppWindowEventArgsPtr pArgs
	)
{
	m_bClosed = true;
}

Sandbox::IViewPtr Sandbox::Provider::CreateView( )
{
	return( alloc_ref_new Sandbox::View );
}

#ifdef REIGN_DESKTOP
Int32 main( _In_ SharedStringArray Args )
#else
[ Platform::MTAThreadAttribute ]
Int32 main( _In_ Platform::Array<Platform::String^>^ Args )
#endif
{
	//#if defined REIGN_DEBUG && defined REIGN_DESKTOP // Debug + Desktop
	//    _CrtSetDbgFlag( // Enable run-time memory check for debug builds.
	//		_CRTDBG_ALLOC_MEM_DF | // Enable debug allocation
	//		_CRTDBG_LEAK_CHECK_DF // Enable checking for memory leaks
	//		);
	//#endif // All configurations

	Sandbox::App::Run( alloc_ref_new Sandbox::Provider( ) );
	return( EXIT_SUCCESS );
}