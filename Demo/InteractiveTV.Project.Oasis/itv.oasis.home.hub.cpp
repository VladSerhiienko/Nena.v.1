#include "app.precompiled.h"
#include "itv.oasis.air.h"
#include "itv.oasis.home.hub.h"


InteractiveTV::Project::Oasis::Home::Hub::Hub(Oasis::Home *master)
	: Oasis::Home::State(master)
	, Oasis::Web::AppBase(nullptr)
{
	Name = _Oasis_origin;
	Name += "home/hub/";

	Context = &Oasis::GetForCurrentThread()->Context;
	Overlay = &Context->Engine->Graphics.d2d;

	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "ctor");
}

InteractiveTV::Project::Oasis::Home::Hub::~Hub()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "dtor");
	// ...
}

void InteractiveTV::Project::Oasis::Home::Hub::Init()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "init");

	CreateDeviceIndependentResources();

	CreateDeviceResources();

	Width = Context->App->View.Fullscreen ? Context->App->View.ScreenWidth : Context->App->View.Width;
	Height = Context->App->View.Fullscreen ? Context->App->View.ScreenHeight : Context->App->View.Height;
	CreateWindowSizeDependentResources();

	CurrentStage = Stage::kSuspended;
	Initted(this);
}

void InteractiveTV::Project::Oasis::Home::Hub::Quit()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "quit");

	if (GetView() && GetView()->IsLoading())
		GetView()->Stop();

	DestroyCoreObjects();

	Quitted(this);
}

void InteractiveTV::Project::Oasis::Home::Hub::OnFrameMove()
{
	if ( CurrentStage == kSuspended )
		return;

	::HRESULT result(S_OK);

	switch ( CurrentStage )
	{
	case Hub::kSuspending:
		break;
	case Hub::kResuming:
		break;
	case Hub::kResumed:
		break;
	}

	result = OnUpdateViewImage();
	if (SUCCEEDED(result))
	{
		D2D1_RECT_F dest = D2D1::RectF(0, 0, Width, Height);

		Overlay->Context->SaveDrawingState(
			BlockState.Get()
			);

		Overlay->Context->BeginDraw();
		Overlay->Context->DrawBitmap(ViewImage.Get(), &dest);
		result = Overlay->Context->EndDraw();

		if (FAILED(result)) _Oasis_air_grabot(this, OasisAirMsg::kError, "frame failed");

		Overlay->Context->RestoreDrawingState(
			BlockState.Get()
			);
	}
}

void InteractiveTV::Project::Oasis::Home::Hub::CreateDeviceResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create device-dependentx resources");

	Overlay->D2DFactory->CreateDrawingStateBlock(
		BlockState.ReleaseAndGetAddressOf()
		);
}

void InteractiveTV::Project::Oasis::Home::Hub::CreateDeviceIndependentResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create device-independent resources");

	CreateCoreObjects();
	GetView()->set_js_method_handler(this);
	GetView()->set_process_listener(this);
	GetView()->set_view_listener(this);
	GetView()->set_load_listener(this);
}

void InteractiveTV::Project::Oasis::Home::Hub::CreateWindowSizeDependentResources()
{
	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo, 
		"create window-size-dependent resources"
		);

	ScreenSize = &Context->Engine->Graphics.d3d.ActualRenderTargetSize;
	AppBase::Width = Context->Engine->Graphics.d3d.LogicalSize.Width;
	AppBase::Height = Context->Engine->Graphics.d3d.LogicalSize.Height;

	if ( !Context->Engine->Graphics.d2d.Device.Get( ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"device not found"
			);

		return;
	}

	::HRESULT result(S_OK);

	auto view = GetView();
	if (!view) result = E_POINTER;
	else GetView()->Resize(Width, Height);

	auto surface = GetViewSurface();
	if (!surface) result = E_POINTER;

	D2D1_BITMAP_PROPERTIES bitmap_properties;
	bitmap_properties.dpiX = Context->Engine->Graphics.d2d.Dpi.x;
	bitmap_properties.dpiY = Context->Engine->Graphics.d2d.Dpi.y;
	bitmap_properties.pixelFormat = D2D1::PixelFormat(
		DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED
		);

	Microsoft::WRL::ComPtr<ID2D1Bitmap> image;
	result = Context->Engine->Graphics.d2d.Context->CreateBitmap(
		D2D1::SizeU(Width, Height), bitmap_properties,
		_Outptr_result_maybenull_ image.ReleaseAndGetAddressOf()
		);
	if ( !Context->Engine->Graphics.d2d.Device.Get( ) )
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"failed to create view bitmap"
			);

		return;
	}
	if (SUCCEEDED(result))
		result = image.As(&ViewImage);
	else
	{
		_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"failed to query bitmap interface"
			);
		return;
	}
}

void InteractiveTV::Project::Oasis::Home::Hub::DiscardDeviceResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "create device-dependent resources");
	BlockState = nullptr;
}

void InteractiveTV::Project::Oasis::Home::Hub::DiscardWindowSizeDependentResources()
{
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "release window-size-dependent resources");
	ViewImage = nullptr;
}

::HRESULT InteractiveTV::Project::Oasis::Home::Hub::OnUpdateViewImage()
{

	::HRESULT result(S_OK);

	::BOOL is_unavailable = IsResizing
		|| !GetView()
		|| GetView()->IsLoading()
		|| GetView()->IsCrashed();

	if (is_unavailable)
	{
		/*_Oasis_air_grabot(
			this, OasisAirMsg::kInfo,
			"webview is pending..."
			);*/

		return E_PENDING;
	}
	else if (auto surface = GetViewSurface())
	{
		__try
		{
			result = ViewImage->CopyFromMemory(
				nullptr, surface->buffer(),
				surface->width() * 4
				);
			return result;
		}
		__except (
			GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
			? EXCEPTION_EXECUTE_HANDLER
			: EXCEPTION_CONTINUE_SEARCH
			)
		{
			::Sleep(1000);
			return E_PENDING;
		}
	}

	return E_FAIL;
}

#pragma region Oasis State

void InteractiveTV::Project::Oasis::Home::Hub::Resume()
{
	if ( GetView( )->IsLoading( ) )
		GetView( )->Stop( );
	GetView( )->LoadURL( Awesomium::WebURL( Awesomium::WSLit( "https://www.google.com/" ) ) );
	GetView()->ResumeRendering(
		);
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"resumed"
		);

	Resumed(this);
}

void InteractiveTV::Project::Oasis::Home::Hub::Suspend()
{
	if ( GetView( )->IsLoading( ) )
		GetView( )->Stop( );
	GetView()->PauseRendering(
		);
	_Oasis_air_grabot(
		this, OasisAirMsg::kWarning,
		"suspended"
		);

	Suspended(this);
}

void InteractiveTV::Project::Oasis::Home::Hub::OnKeyPressed(::UINT32 k)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnKeyReleased(::UINT32 k)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnMouseMoved(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnMouseLBPressed(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnMouseRBPressed(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnMouseLBReleased(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnMouseRBReleased(::FLOAT x, ::FLOAT y)
{
}

void InteractiveTV::Project::Oasis::Home::Hub::OnResized()
{
	Width = Context->Engine->Graphics.d3d.ActualRenderTargetSize.Width;
	Height = Context->Engine->Graphics.d3d.ActualRenderTargetSize.Height;

	_Oasis_air_grabot(
		this, OasisAirMsg::kInfo, 
		"on resized %d %d", 
		Width, Height
		);

	GetView()->Resize(Width, Height);
	::Sleep(500);

	Context->Web->OnFrameMove();
	CreateWindowSizeDependentResources();
}

#pragma endregion


#pragma region Event overrides

#pragma region View events

//! This event occurs when the page title has changed.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeTitle(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& title
	)
{
}

//! This event occurs when the page URL has changed.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeAddressBar(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the tooltip text has changed. You
//! should hide the tooltip when the text is empty.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeTooltip(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& tooltip
	)
{
	//Oasis::Web::State::View;
}

//! This event occurs when the target URL has changed. This
//! is usually the result of hovering over a link on a page.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeTargetURL(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the cursor has changed. This is
//! is usually the result of hovering over different content.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeCursor(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::Cursor cursor
	)
{
}

//! This event occurs when the focused element changes on the page.
//! This is usually the result of textbox being focused or some other
//! user-interaction event.
void InteractiveTV::Project::Oasis::Home::Hub::OnChangeFocus(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::FocusedElementType focused_type
	)
{
}

//! This event occurs when a message is added to the console on the page.
//! This is usually the result of a JavaScript error being encountered
//! on a page.
void InteractiveTV::Project::Oasis::Home::Hub::OnAddConsoleMessage(
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
void InteractiveTV::Project::Oasis::Home::Hub::OnShowCreatedWebView(
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
void InteractiveTV::Project::Oasis::Home::Hub::OnBeginLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ bool is_error_page
	)
{
	auto url_utf8 = Awesomium::ToString(url.spec());
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "loading frame %s", url_utf8.c_str());
}

//! This event occurs when a frame fails to load. See error_desc for additional information.
void InteractiveTV::Project::Oasis::Home::Hub::OnFailLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ int error_code,
	_In_ const Awesomium::WebString& error_desc
	)
{
	auto url_utf8 = Awesomium::ToString(url.spec());
	_Oasis_air_grabot(this, OasisAirMsg::kWarning, "failed frame %s", url_utf8.c_str());
}

//! This event occurs when the page finishes loading a frame. The main frame always finishes loading last for a given page load.
void InteractiveTV::Project::Oasis::Home::Hub::OnFinishLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url
	)
{
	auto url_utf8 = Awesomium::ToString(url.spec());
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "finish frame %s", url_utf8.c_str());
}

//! This event occurs when the DOM has finished parsing and the window object is available for JavaScript execution.
void InteractiveTV::Project::Oasis::Home::Hub::OnDocumentReady(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
	auto url_utf8 = Awesomium::ToString(url.spec());
	_Oasis_air_grabot(this, OasisAirMsg::kInfo, "constructed %s", url_utf8.c_str());
}

#pragma endregion // load events

#pragma region Process events

//! This event occurs when the process hangs.
void InteractiveTV::Project::Oasis::Home::Hub::OnUnresponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process becomes responsive after a hang.
void InteractiveTV::Project::Oasis::Home::Hub::OnResponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process crashes.
void InteractiveTV::Project::Oasis::Home::Hub::OnCrashed(
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
void InteractiveTV::Project::Oasis::Home::Hub::OnMethodCall(
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
Awesomium::JSValue InteractiveTV::Project::Oasis::Home::Hub::OnMethodCallWithReturnValue(
	_In_ Awesomium::WebView* caller,
	_In_ unsigned int remote_object_id,
	_In_ const Awesomium::WebString& method_name,
	_In_ const Awesomium::JSArray& args
	)
{
	return Awesomium::JSValue();
}

#pragma endregion // javascript events

#pragma endregion