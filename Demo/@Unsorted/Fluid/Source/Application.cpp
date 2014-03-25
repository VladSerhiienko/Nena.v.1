#include "Precompiled.h"
#include "Application.h"

void Demo::View::Load( _In_ Demo::View::EntryPointString wszEntryPoint )
{
}

void Demo::View::Uninitialize( )
{
#ifdef REIGN_DESKTOP
	Safe_Delete(m_RendererPtr);
#endif
}

void Demo::View::Initialize( _In_ Demo::AppViewPtr ApplicationView )
{
	m_bActivated = false;
	m_bClosed = false;
	m_bVisible = false;

#ifdef REIGN_DESKTOP
	m_RendererPtr = new FluidRenderer();
#else
	m_RendererPtr = ref new Demo::FluidRenderer();
#endif
}

void Demo::View::SetWindow( _In_ Demo::AppWindowPtr Window )
{
	Window->VisibilityChanged += ref new Demo::View::AppWindowVisiblityEventHandler( this, &Demo::View::OnWindowVisibilityChanged );
	Window->Activated += ref new Demo::View::AppWindowActivationEventHandler( this, &Demo::View::OnWindowActivatedChanged );
	Window->SizeChanged += ref new Demo::View::AppWindowSizingEventHandler( this, &Demo::View::OnWindowSizeChanged );
	Window->Closed += ref new Demo::View::AppWindowEventHandler( this, &Demo::View::OnWindowClosed );
	/*Window->PointerReleased += ref new Demo::View::AppWindowPointerEventHandler( this, &Demo::View::OnPointerReleased );
	Window->PointerPressed += ref new Demo::View::AppWindowPointerEventHandler( this, &Demo::View::OnPointerPressed );
	Window->PointerMoved += ref new Demo::View::AppWindowPointerEventHandler( this, &Demo::View::OnPointerMoved );*/

#ifdef REIGN_DESKTOP
	m_RendererPtr->Initialize( Window->GetHandle(), Window->GetLogicalDpi() );
	//m_RendererPtr->Initialize( Window->GetHandle(), Window->GetLogicalDpi(), D3D_DRIVER_TYPE_REFERENCE );
#else
	m_RendererPtr->Initialize( Window, Windows::Graphics::DisplayProperties::LogicalDpi );
#endif
	m_RendererPtr->CreateBuffers();
	m_RendererPtr->CreateShaders();
}

void Demo::View::Run( )
{
	Demo::AppWindow::GetForCurrentThread()->Activate();

	while ( !m_bClosed )
	{
		Demo::AppWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(
			AppWindowProcessEventsOption::ProcessAllIfPresent
			);

		m_RendererPtr->OnFrameMove();
		m_RendererPtr->OnFrame();
	}
}

void Demo::View::OnPointerMoved( 
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}

void Demo::View::OnPointerPressed( 
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}
void Demo::View::OnPointerReleased( 
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowPointerEventArgsPtr pArgs
	)
{
}

void Demo::View::OnWindowSizeChanged(
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowSizingEventArgsPtr pArgs
	)
{
	m_RendererPtr->UpdateForWindowSizeChange();
}

void Demo::View::OnWindowActivatedChanged(
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowActivationEventArgsPtr pArgs
	)
{
	m_bActivated = pArgs->WindowActivationState != Demo::View::AppWindowActivationState::Deactivated;
}

void Demo::View::OnWindowVisibilityChanged(
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowVisibilityEventArgsPtr pArgs
	)
{
	m_bVisible = pArgs->Visible;
}

void Demo::View::OnWindowClosed(
	_In_ Demo::AppWindowPtr pWindow,
	_In_ Demo::View::AppWindowEventArgsPtr pArgs
	)
{
	m_bClosed = true;
}

Demo::IViewPtr Demo::Provider::CreateView( )
{
	return( ref new Demo::View );
}

#ifdef REIGN_DESKTOP
Int32 main( _In_ SharedStringArray Args )
#else
[Platform::MTAThreadAttribute]
Int32 main( _In_ Platform::Array<Platform::String^>^ Args )
#endif
{
#if defined REIGN_DEBUG && defined REIGN_DESKTOP // Debug + Desktop
    _CrtSetDbgFlag( // Enable run-time memory check for debug builds.
		_CRTDBG_ALLOC_MEM_DF | // Enable debug allocation
		_CRTDBG_LEAK_CHECK_DF // Enable checking for memory leaks
		);
#endif // All configurations

	Demo::App::Run( // Pass the app view provider to core app
		ref new Demo::Provider() // App view provider
		);

	return( EXIT_SUCCESS );
}
