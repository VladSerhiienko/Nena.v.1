#include "app.precompiled.h"
#include "itv.oasis.home.h"

InteractiveTV::Project::Oasis::Home::Home()
	: Oasis::State(nullptr)
	, Oasis::Web::AppBase(nullptr)
	, Overlay(nullptr)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::Home()\n");
	Context = &Oasis::GetForCurrentThread()->Context;
	Overlay = &Context->Engine->Graphics.d2d;
}

InteractiveTV::Project::Oasis::Home::~Home()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::~Home()\n");
	Quit();
}

void InteractiveTV::Project::Oasis::Home::Init()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::Init()\n");

	CreateDeviceIndependentResources();

	CreateDeviceResources();

	Width = Context->App->View.Fullscreen ? Context->App->View.ScreenWidth : Context->App->View.Width;
	Height = Context->App->View.Fullscreen ? Context->App->View.ScreenHeight : Context->App->View.Height;
	CreateWindowSizeDependentResources();

	//GetView()->LoadURL(Awesomium::WebURL(Awesomium::WSLit("http://html.adobe.com/edge/animate/")));
	GetView()->LoadURL(Awesomium::WebURL(Awesomium::WSLit("http://syddev.com/jquery.shadow/")));
	//GetView()->LoadURL(Awesomium::WebURL(Awesomium::WSLit("https://www.google.com.ua/")));
	// http://syddev.com/jquery.shadow/
	// http://www.w3schools.com/css/css3_animations.asp

	Initted(this);
}

void InteractiveTV::Project::Oasis::Home::Quit()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::Quit()\n");
	GetView()->Stop();
	Quitted(this);
}

void InteractiveTV::Project::Oasis::Home::OnFrameMove()
{
	::HRESULT result(S_OK);

	result = OnUpdateViewImage();
	if (SUCCEEDED(result))
	{
		D2D1_RECT_F dest = D2D1::RectF(0, 0, Width, Height);
		//D2D1_RECT_F dest = D2D1::RectF(0, 0, Width / 2, Height / 2);

		Overlay->Context->SaveDrawingState(BlockState.Get());
		Overlay->Context->BeginDraw();
		//Overlay->Context->DrawBitmap(ViewImage.Get(), &dest); // , 0.8f);
		Overlay->Context->DrawBitmap(ViewImage.Get());
		result = Overlay->Context->EndDraw();


		if (FAILED(result)) OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnFrameMove() [FAILED]\n");

		Overlay->Context->RestoreDrawingState(BlockState.Get());
	}
}

::HRESULT InteractiveTV::Project::Oasis::Home::CreateDeviceResources()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::CreateDeviceResources()\n");

	Overlay->D2DFactory->CreateDrawingStateBlock(BlockState.ReleaseAndGetAddressOf());


	return E_NOTIMPL;
}

::HRESULT InteractiveTV::Project::Oasis::Home::CreateDeviceIndependentResources()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::CreateDeviceIndependentResources()\n");

	CreateCoreObjects();
	GetView()->set_js_method_handler(this);
	GetView()->set_process_listener(this);
	GetView()->set_view_listener(this);
	GetView()->set_load_listener(this);

	return E_NOTIMPL;
}

::HRESULT InteractiveTV::Project::Oasis::Home::CreateWindowSizeDependentResources()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::CreateWindowSizeDependentResources()\n");

	if (!Context->Engine->Graphics.d2d.Device.Get()) return E_POINTER;
	Width = Context->Engine->Graphics.d3d.LogicalSize.Width;
	Height = Context->Engine->Graphics.d3d.LogicalSize.Height;

	::HRESULT result(S_OK);

	auto view = GetView();
	if (!view) result = E_POINTER;
	else GetView()->Resize(Width, Height);

	auto surface = GetViewSurface();
	if (!surface) result = E_POINTER;

	if (SUCCEEDED(result))
	{
		D2D1_BITMAP_PROPERTIES1 bitmap_properties;
		bitmap_properties.dpiX = Context->Engine->Graphics.d2d.Dpi.x;
		bitmap_properties.dpiY = Context->Engine->Graphics.d2d.Dpi.y;
		bitmap_properties.bitmapOptions = D2D1_BITMAP_OPTIONS::D2D1_BITMAP_OPTIONS_NONE;
		bitmap_properties.colorContext = NULL;
		bitmap_properties.pixelFormat = D2D1::PixelFormat(
			DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE::D2D1_ALPHA_MODE_PREMULTIPLIED
			);

		result = Context->Engine->Graphics.d2d.Context->CreateBitmap(
			D2D1::SizeU(surface->width(), surface->height()),
			surface->buffer(), surface->width() / 2, bitmap_properties,
			_Outptr_result_maybenull_ ViewImage.ReleaseAndGetAddressOf()
			);
	}
	else
	{
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

		if (SUCCEEDED(result))
		result = image.As(&ViewImage);
	}

	return result;
}

::HRESULT InteractiveTV::Project::Oasis::Home::OnUpdateViewImage()
{

	::HRESULT result(S_OK);

	if (GetView()->IsLoading())
	{
		//OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnUpdateViewImage() Loading...\n");
	}
	else if (auto surface = GetViewSurface())
	{
		//GetView()->InjectMouseWheel(-5, 0);
		result = ViewImage->CopyFromMemory(
			nullptr, surface->buffer(),
			surface->width() * 4 // / 2
			);
	}
	else result = E_POINTER;

	return result;
}

#pragma region Oasis State

void InteractiveTV::Project::Oasis::Home::Resume()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::Resume()\n");

	GetView()->ResumeRendering();
	Resumed(this);
}

void InteractiveTV::Project::Oasis::Home::Suspend()
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::Suspend()\n");

	GetView()->PauseRendering();
	Suspended(this);
}

void InteractiveTV::Project::Oasis::Home::OnStateQuitted(_In_ Oasis::State *kid)
{
}

void InteractiveTV::Project::Oasis::Home::OnStateResumed(_In_ Oasis::State *kid)
{
}

void InteractiveTV::Project::Oasis::Home::OnStateSuspended(_In_ Oasis::State *kid)
{
}

void InteractiveTV::Project::Oasis::Home::OnViewClosed(_In_ Oasis::State::View *host_window)
{
}

void InteractiveTV::Project::Oasis::Home::OnViewClosing(_In_ Oasis::State::View *host_window)
{
	Oasis::Web::AppBase::DestroyCoreObjects();
}

void InteractiveTV::Project::Oasis::Home::OnViewToggled(_In_ Oasis::State::View *host_window)
{
	OnViewResizedMoved(host_window);
}

void InteractiveTV::Project::Oasis::Home::OnViewResizedMoved(_In_ Oasis::State::View *host_window)
{
	Width = Context->Engine->Graphics.d3d.LogicalSize.Width;
	Height = Context->Engine->Graphics.d3d.LogicalSize.Height;
	//Width = host_window->Fullscreen ? host_window->ScreenWidth : host_window->Width;
	//Height = host_window->Fullscreen ? host_window->ScreenHeight : host_window->Height;
	GetView()->Resize(Width, Height);
	CreateWindowSizeDependentResources();
}

#pragma endregion

#pragma region Event overrides

#pragma region View events

//! This event occurs when the page title has changed.
void InteractiveTV::Project::Oasis::Home::OnChangeTitle(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& title
	)
{
}

//! This event occurs when the page URL has changed.
void InteractiveTV::Project::Oasis::Home::OnChangeAddressBar(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the tooltip text has changed. You
//! should hide the tooltip when the text is empty.
void InteractiveTV::Project::Oasis::Home::OnChangeTooltip(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebString& tooltip
	)
{
	//Oasis::Web::State::View;
}

//! This event occurs when the target URL has changed. This
//! is usually the result of hovering over a link on a page.
void InteractiveTV::Project::Oasis::Home::OnChangeTargetURL(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
}

//! This event occurs when the cursor has changed. This is
//! is usually the result of hovering over different content.
void InteractiveTV::Project::Oasis::Home::OnChangeCursor(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::Cursor cursor
	)
{
}

//! This event occurs when the focused element changes on the page.
//! This is usually the result of textbox being focused or some other
//! user-interaction event.
void InteractiveTV::Project::Oasis::Home::OnChangeFocus(
	_In_ Awesomium::WebView* caller,
	_In_ Awesomium::FocusedElementType focused_type
	)
{
}

//! This event occurs when a message is added to the console on the page.
//! This is usually the result of a JavaScript error being encountered
//! on a page.
void InteractiveTV::Project::Oasis::Home::OnAddConsoleMessage(
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
void InteractiveTV::Project::Oasis::Home::OnShowCreatedWebView(
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
void InteractiveTV::Project::Oasis::Home::OnBeginLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ bool is_error_page
	)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnBeginLoadingFrame()\n");
}

//! This event occurs when a frame fails to load. See error_desc for additional information.
void InteractiveTV::Project::Oasis::Home::OnFailLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url,
	_In_ int error_code,
	_In_ const Awesomium::WebString& error_desc
	)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnFailLoadingFrame()\n");
}

//! This event occurs when the page finishes loading a frame. The main frame always finishes loading last for a given page load.
void InteractiveTV::Project::Oasis::Home::OnFinishLoadingFrame(
	_In_ Awesomium::WebView* caller,
	_In_ int64 frame_id,
	_In_ bool is_main_frame,
	_In_ const Awesomium::WebURL& url
	)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnFinishLoadingFrame()\n");
}

//! This event occurs when the DOM has finished parsing and the window object is available for JavaScript execution.
void InteractiveTV::Project::Oasis::Home::OnDocumentReady(
	_In_ Awesomium::WebView* caller,
	_In_ const Awesomium::WebURL& url
	)
{
	OutputDebugStringA("InteractiveTV::Project::Oasis::Home::OnDocumentReady()\n");
	auto url_utf8 = Awesomium::ToString(url.spec());
}

#pragma endregion // load events

#pragma region Process events

//! This event occurs when the process hangs.
void InteractiveTV::Project::Oasis::Home::OnUnresponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process becomes responsive after a hang.
void InteractiveTV::Project::Oasis::Home::OnResponsive(
	_In_ Awesomium::WebView* caller
	)
{
}

//! This event occurs when the process crashes.
void InteractiveTV::Project::Oasis::Home::OnCrashed(
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
void InteractiveTV::Project::Oasis::Home::OnMethodCall(
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
Awesomium::JSValue InteractiveTV::Project::Oasis::Home::OnMethodCallWithReturnValue(
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
