#include "app.precompiled.h"
#include "itv.oasis.air.h"
#include "itv.oasis.home.hub.h"
#include "itv.oasis.home.start.h"

#define _Oasis_home_hub "home/hub/"

InteractiveTV::Oasis::Home::HubScreen::HubScreen( Oasis::Home *master )
: Oasis::Home::State( master )
, Oasis::Web::AppBase( nullptr )
{
	Name = _Oasis_origin _Oasis_home_hub;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"ctor"
		);
}

InteractiveTV::Oasis::Home::HubScreen::~HubScreen( )
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"dtor"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::Init( )
{
	Img.Bitmap.AcquireGlobals( );

	Navdialog.dialog_pointers = Pointers;
	Navdialog.AddButton( "./Assets/News.png" );
	Navdialog.AddButton( "./Assets/Business-Card-01.png" );
	Navdialog.AddButton( "./Assets/Debug .png" );
	Navdialog.AddButton( "./Assets/QR-Code.png" );
	Navdialog.AddButton( "./Assets/Media-Player.png" );
	Navdialog.AddButton( "./Assets/Confused.png" );
	Navdialog.Init( );

	Navdialog.button_selected += Nena_Delegate_MakeBind(
		this, &HubScreen::OnDialogButtonSelected
		);

	Easing.SetTotal( 2.0f );
	Easing.SuspendingValue.Params.Distance = 0.0f;
	Easing.SuspendingValue.Params.Offset = 2.0f;
	Easing.ResumingValue.Params.Distance = -2.0f;
	Easing.ResumingValue.Params.Offset = 2.0f;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"init"
		);

	Initted( this );
}

void InteractiveTV::Oasis::Home::HubScreen::Quit( )
{
	if ( GetView( ) &&
		GetView( )->IsLoading( ) )
		GetView( )->Stop( );
	DestroyCoreObjects( );

	Navdialog.Quit( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"quit"
		);

	Quitted( this );
}

::HRESULT InteractiveTV::Oasis::Home::HubScreen::OnUpdateViewImage( )
{
	::HRESULT result( S_OK );

	::BOOL is_unavailable = !GetView( )
		|| GetView( )->IsLoading( )
		|| GetView( )->IsCrashed( );

	if ( is_unavailable ) return E_PENDING;
	else if ( auto surface = GetViewSurface( ) )
	{
		__try
		{
			result = Img.Bitmap.img_bitmap->CopyFromMemory(
				nullptr, // completely overwritten
				surface->buffer( ), // bytes
				surface->width( ) * 4 // pitch
				);
			return result;
		}
		__except (
			GetExceptionCode( ) == EXCEPTION_ACCESS_VIOLATION
			? EXCEPTION_EXECUTE_HANDLER
			: EXCEPTION_CONTINUE_SEARCH
			)
		{
			::Sleep( 500 );
			return E_PENDING;
		}
	}

	return E_FAIL;
}

InteractiveTV::Oasis::Home::HubScreen::Stage
InteractiveTV::Oasis::Home::HubScreen::GetEasingStage(
)
{
	return Easing.CurrentStage;
}

void InteractiveTV::Oasis::Home::HubScreen::OnDialogButtonSelected(
	::InteractiveTV::Ui::Round::Dialog *dlg,
	::InteractiveTV::Ui::Round::Dialog::Button *btn
	)
{
	Pointers->IsVisible[ 0 ] = FALSE;
	if ( btn->button_id == 4 )
	{
		Start->Resume( );
		Suspend( );
	}
}

void InteractiveTV::Oasis::Home::HubScreen::OnFrameMove( )
{
	if ( GetEasingStage( ) != Stage::kSuspended )
	if ( SUCCEEDED( OnUpdateViewImage( ) ) )
	{
		::Nena::Vector2 transaction_scene;
		Easing += Img.Bitmap.cross_context->Timer.Elapsed;
		transaction_scene.x = Easing.OnFrame(
			);
		Img.Bitmap.CalculateActualPositionMetricTs(
			transaction_scene
			);

		Img.Bitmap.overlay->Context->SetTransform(
			::D2D1::Matrix3x2F::Translation( { transaction_scene.x, 0 } )
			);
		Img.Bitmap.DrawFullscreen(
			);
		Img.Bitmap.RestoreDeviceTransform(
			);
		Navdialog.OnFrameMove(
			);
	}
}

void InteractiveTV::Oasis::Home::HubScreen::CreateDeviceResources( )
{
	Img.Bitmap.CreateDeviceResources(
		);
	Navdialog.CreateDeviceResources(
		);

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-dependentx resources created"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::CreateDeviceIndependentResources( )
{
	CreateCoreObjects(
		);

	GetView( )->set_js_method_handler( this );
	GetView( )->set_process_listener( this );
	GetView( )->set_view_listener( this );
	GetView( )->set_load_listener( this );
	GetView( )->LoadURL( Awesomium::WebURL(
		Awesomium::WSLit( "https://www.google.com/" )
		) );

	Navdialog.CreateDeviceIndependentResources(
		);

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"device-independent resources created"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::CreateWindowSizeDependentResources( )
{
	auto screen_size = &Img.Bitmap.cross_context->Engine->Graphics.d3d.ActualRenderTargetSize;
	AppBase::Width = (INT32) screen_size->Width;
	AppBase::Height = (INT32) screen_size->Height;

	if ( !Context->Engine->Graphics.d2d.Device.Get( ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"device not found"
			);

		return;
	}

	::HRESULT result( S_OK );

	auto view = GetView( );
	if ( !view ) result = E_POINTER;
	else GetView( )->Resize( Width, Height );

	auto surface = GetViewSurface( );
	if ( !surface ) result = E_POINTER;

	D2D1_BITMAP_PROPERTIES bitmap_properties;
	bitmap_properties.dpiX = Context->Engine->Graphics.d2d.Dpi.x;
	bitmap_properties.dpiY = Context->Engine->Graphics.d2d.Dpi.y;
	bitmap_properties.pixelFormat = D2D1::PixelFormat(
		DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED
		);

	Microsoft::WRL::ComPtr<ID2D1Bitmap> image;
	result = Context->Engine->Graphics.d2d.Context->CreateBitmap(
		D2D1::SizeU( Width, Height ), bitmap_properties,
		_Outptr_result_maybenull_ image.ReleaseAndGetAddressOf( )
		);
	if ( FAILED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"failed to create view bitmap"
			);

		return;
	}

	result = image.As( &Img.Bitmap.img_bitmap );
	if ( SUCCEEDED( result ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"failed to query bitmap interface"
			);
		return;
	}

	Navdialog.CreateWindowSizeDependentResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"window-size-dependent resources created"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::DiscardDeviceResources( )
{
	Navdialog.DiscardDeviceResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"create device-dependent resources"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::DiscardWindowSizeDependentResources( )
{
	Navdialog.DiscardWindowSizeDependentResources( );

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"release window-size-dependent resources"
		);

	Img.Bitmap.img_bitmap = nullptr;
}

#pragma region Oasis State

void InteractiveTV::Oasis::Home::HubScreen::Resume( )
{
	if ( GetView( ) )
		GetView( )->ResumeRendering( );

	Easing.Resume( );
	Pointers->IsVisible[ 0 ] = FALSE;

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::Suspend( )
{
	if ( GetView( ) )
	{
		if ( GetView( )->IsLoading( ) )
			GetView( )->Stop( );
		GetView( )->PauseRendering( );
	}

	Easing.Suspend( );
	Pointers->IsVisible[ 0 ] = TRUE;

	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);
}

void InteractiveTV::Oasis::Home::HubScreen::OnGestureReceived(
	Remote::Input::GestureAppMessageArg arg
	)
{
	if ( GetEasingStage( ) != Stage::kSuspended )
	{
		if ( Navdialog.GetEasingStage( ) == ::Nena::Animation::EasingStage::kResumed )
			Navdialog.OnGestureReceived( arg );

		switch ( arg )
		{
		case Remote::Input::kWave:
			break;
		case Remote::Input::kCircle:
			if ( Navdialog.GetEasingStage( ) == ::Nena::Animation::EasingStage::kSuspended )
				Pointers->IsVisible[ 0 ] = TRUE,
				Navdialog.Resume( );

			break;
		case Remote::Input::kNavigationSwipeUp:
			break;
		case Remote::Input::kNavigationSwipeDown:
			break;
		case Remote::Input::kNavigationSwipeLeft:
			break;
		case Remote::Input::kNavigationSwipeRight:
			break;
		}
	}
}

void InteractiveTV::Oasis::Home::HubScreen::OnResized( )
{
	Width = (INT32) Img.Bitmap.cross_context->Engine->Graphics.d3d.ActualRenderTargetSize.Width;
	Height = (INT32) Img.Bitmap.cross_context->Engine->Graphics.d3d.ActualRenderTargetSize.Height;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo,
		"on resized %d %d",
		Width, Height
		);

	GetView( )->Resize(
		Width, Height
		);

	::Sleep( 500 );
	Context->Web->OnFrameMove( );
	CreateWindowSizeDependentResources( );
	Navdialog.OnResized( );

}

#pragma endregion

#pragma region Event overrides

#pragma region View events

//! This event occurs when the page title has changed.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeTitle(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& title
	)
{
}

//! This event occurs when the page URL has changed.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeAddressBar(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the tooltip text has changed. You
//! should hide the tooltip when the text is empty.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeTooltip(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& tooltip
	)
{
	//Oasis::Web::State::View;
}

//! This event occurs when the target URL has changed. This
//! is usually the result of hovering over a link on a page.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeTargetURL(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the cursor has changed. This is
//! is usually the result of hovering over different content.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeCursor(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::Cursor cursor
	)
{
}

//! This event occurs when the focused element changes on the page.
//! This is usually the result of textbox being focused or some other
//! user-interaction event.
void InteractiveTV::Oasis::Home::HubScreen::OnChangeFocus(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::FocusedElementType focused_type
	)
{
}

//! This event occurs when a message is added to the console on the page.
//! This is usually the result of a JavaScript error being encountered
//! on a page.
void InteractiveTV::Oasis::Home::HubScreen::OnAddConsoleMessage(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& message,
	_In_ int line_number,
	_In_ const Awesomium::WebString& source
	)
{
}

//! This event occurs when a WebView creates a new child WebView
//! (usually the result of window.open or an external link). It
//! is your responsibility to display this child WebView in your
//! application. You should call Resize on the child WebView
//! immediately after this event to make it match your container
//! size.
//!
//! If this is a child of a Windowed WebView, you should call
//! WebView::set_parent_window on the new view immediately within
//! this event.
void InteractiveTV::Oasis::Home::HubScreen::OnShowCreatedWebView(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::WebView* new_view,
	_In_ const Awesomium::WebURL& opener_url,
	_In_ const Awesomium::WebURL& target_url,
	_In_ const Awesomium::Rect& initial_pos,
	_In_ bool is_popup
	)
{
}

#pragma endregion // view events

#pragma region Load events

//! This event occurs when the page begins loading a frame.
void InteractiveTV::Oasis::Home::HubScreen::OnBeginLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ bool is_error_page
	)
{
	auto url_utf8 = Awesomium::ToString( url.spec( ) );
	_Oasis_air_grabot( this, OasisAirMsg::kInfo, "loading frame %s", url_utf8.c_str( ) );
}

//! This event occurs when a frame fails to load. See error_desc for additional information.
void InteractiveTV::Oasis::Home::HubScreen::OnFailLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ int error_code,
	_In_ const Awesomium::WebString& error_desc
	)
{
	auto url_utf8 = Awesomium::ToString( url.spec( ) );
	_Oasis_air_grabot( this, OasisAirMsg::kWarning, "failed frame %s", url_utf8.c_str( ) );
}

//! This event occurs when the page finishes loading a frame. The main frame always finishes loading last for a given page load.
void InteractiveTV::Oasis::Home::HubScreen::OnFinishLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url
	)
{
	auto url_utf8 = Awesomium::ToString( url.spec( ) );
	_Oasis_air_grabot( this, OasisAirMsg::kInfo, "finish frame %s", url_utf8.c_str( ) );
}

//! This event occurs when the DOM has finished parsing and the window object is available for JavaScript execution.
void InteractiveTV::Oasis::Home::HubScreen::OnDocumentReady(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
	auto url_utf8 = Awesomium::ToString( url.spec( ) );
	_Oasis_air_grabot( this, OasisAirMsg::kInfo, "constructed %s", url_utf8.c_str( ) );
}

#pragma endregion // load events

#pragma region Process events

//! This event occurs when the process hangs.
void InteractiveTV::Oasis::Home::HubScreen::OnUnresponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process becomes responsive after a hang.
void InteractiveTV::Oasis::Home::HubScreen::OnResponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process crashes.
void InteractiveTV::Oasis::Home::HubScreen::OnCrashed(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::TerminationStatus status
	)
{
}

#pragma endregion // process events

#pragma region Javascript events

//! This event occurs whenever a custom JSObject method (with no return
//! value) is called from JavaScript.
//! @param  caller  The WebView that dispatched this event.
//! @param  remote_object_id  The unique ID of the JS Object that contains this method.
//! @param  method_name  The name of the method being called.
//! @param  args  The arguments passed to the method.
void InteractiveTV::Oasis::Home::HubScreen::OnMethodCall(
	Awesomium::WebView* caller,
	unsigned int remote_object_id,
	const Awesomium::WebString& method_name,
	const Awesomium::JSArray& args
	)
{

}

//! This event occurs whenever a custom JSObject method (with a return
//! value) is called from JavaScript.
//!  
//! @param  caller  The WebView that dispatched this event.
//! @param  remote_object_id  The unique ID of the JS Object that contains this method.
//! @param  method_name  The name of the method being called.
//! @param  args  The arguments passed to the method.
//! @return  You should handle this method call and return the result.
//!  
//! @note  
//!        This event is synchronous (the WebView process is blocked until
//!        this method returns). You must be careful not to invoke any
//!        synchronous API calls at the risk of causing deadlock.
//!        This event does not support objects being passed as arguments
//!        (all JS objects are replaced with undefined in the args array).
Awesomium::JSValue InteractiveTV::Oasis::Home::HubScreen::OnMethodCallWithReturnValue(
	_In_ Awesomium::WebView* caller,
	_In_ unsigned int remote_object_id,
	_In_ const Awesomium::WebString& method_name,
	_In_ const Awesomium::JSArray& args
	)
{
	return Awesomium::JSValue( );
}

#pragma endregion // javascript events

#pragma endregion

void InteractiveTV::Oasis::Home::HubScreen::OnKeyPressed( ::UINT32 k )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnKeyReleased( ::UINT32 k )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnMouseMoved( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnMouseLBPressed( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnMouseRBPressed( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnMouseLBReleased( ::FLOAT x, ::FLOAT y )
{
}

void InteractiveTV::Oasis::Home::HubScreen::OnMouseRBReleased( ::FLOAT x, ::FLOAT y )
{
}