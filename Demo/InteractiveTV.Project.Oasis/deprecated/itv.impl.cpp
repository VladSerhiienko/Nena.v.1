#include "app.precompiled.h"
#include "itv.h"

Nena::InteractiveTV::Web::View::Notify::ProcessEventArgs::~ProcessEventArgs() {}
Nena::InteractiveTV::Web::View::Notify::LoadEventArgs::~LoadEventArgs() {}

typedef struct Nena::InteractiveTV::Web::Implementation
{
	friend Web::View::Implementation;
	friend Web::Session::Implementation;

	InteractiveTV::Web *core_public;
	InteractiveTV::Web::View view_public_default;
	InteractiveTV::Web::Session session_public_default;

	Awesomium::WebCore *core_private;
	Awesomium::WebConfig configuration;

	Implementation(Web *core_public)
		: configuration()
		, core_private(nullptr)
		, core_public(core_public)
		, view_public_default(core_public)
		, session_public_default(core_public)
	{
	}

	~Implementation()
	{
	}

	//! @note configuration should be set
	void Initialize()
	{
		if (core_private) return;
		core_private = Awesomium::WebCore::Initialize(configuration);

		session_public_default.Initialize();
		view_public_default.Initialize(nullptr, &session_public_default);
	}

	void Release()
	{
		view_public_default.Release();
		session_public_default.Release();
		Awesomium::WebCore::Shutdown();
		core_private = nullptr;
	}

};

#pragma endregion

#pragma region Private web session

typedef struct Nena::InteractiveTV::Web::Session::Implementation
{
	friend Web::Implementation;
	friend Web::View::Implementation;

	InteractiveTV::Web::Session *session_public;
	Awesomium::WebSession *session_private;
	Awesomium::WebPreferences preferencies;

	Implementation(Session *session_public)
		: preferencies()
		, session_private(nullptr)
		, session_public(session_public)
	{
	}

	~Implementation()
	{
		session_public = nullptr;
		Release();
	}

	//! @note preferencies should be set
	void Initialize(_In_ Web::Implementation *web, _In_ std::string path)
	{
		if (session_private) return;
		session_private = web->core_private->CreateWebSession(
			// where web resources will be stored
			Awesomium::WebString::CreateFromUTF8(path.c_str(), path.size()),
			preferencies
			);
	}

	void Release()
	{
		if (session_private)
			session_private->Release(),
			session_private = nullptr;
	}

};

#pragma endregion

#pragma region Private web view

typedef struct Nena::InteractiveTV::Web::View::Implementation 
	: public Awesomium::JSMethodHandler
	, public Awesomium::WebViewListener::View
	, public Awesomium::WebViewListener::Load
	, public Awesomium::WebViewListener::Process
{
	friend Web::Implementation;
	friend Web::Session::Implementation;

	Web::View::Notify::ProcessEventArgs process_event_args;
	Web::View::Notify::LoadEventArgs load_event_args;

	InteractiveTV::Web::View *view_public;
	Awesomium::WebView *view_private;

	Implementation(Nena::InteractiveTV::Web::View *view_public)
		: view_private(nullptr)
		, view_public(view_public)
	{
	}

	~Implementation()
	{
		view_public = nullptr;
		Release();
	}

	void Initialize(
		_In_ Web::Implementation *web,
		_In_ INT32 view_width, _In_ INT32 view_height,
		_In_ Web::Session::Implementation *resources
		)
	{
		if (view_private) return;
		view_private = web->core_private->CreateWebView(
			view_width, view_height, resources->session_private,
			Awesomium::WebViewType::kWebViewType_Offscreen
			),
			// set event handlers for the current view
			view_private->set_view_listener(this),
			view_private->set_load_listener(this),
			view_private->set_process_listener(this),
			view_private->set_js_method_handler(this);
	}

	void Release()
	{
		if (view_private)
			view_private->Destroy(),
			view_private = nullptr;
	}

#pragma region Event overrides

#pragma region View events

	//! This event occurs when the page title has changed.
	virtual void OnChangeTitle(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebString& title
		)
	{
	}

	//! This event occurs when the page URL has changed.
	virtual void OnChangeAddressBar(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		)
	{
	}

	//! This event occurs when the tooltip text has changed. You
	//! should hide the tooltip when the text is empty.
	virtual void OnChangeTooltip(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebString& tooltip
		)
	{
	}

	//! This event occurs when the target URL has changed. This
	//! is usually the result of hovering over a link on a page.
	virtual void OnChangeTargetURL(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		)
	{
	}

	//! This event occurs when the cursor has changed. This is
	//! is usually the result of hovering over different content.
	virtual void OnChangeCursor(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::Cursor cursor
		)
	{
	}

	//! This event occurs when the focused element changes on the page.
	//! This is usually the result of textbox being focused or some other
	//! user-interaction event.
	virtual void OnChangeFocus(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::FocusedElementType focused_type
		)
	{
	}

	//! This event occurs when a message is added to the console on the page.
	//! This is usually the result of a JavaScript error being encountered
	//! on a page.
	virtual void OnAddConsoleMessage(
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
	virtual void OnShowCreatedWebView(
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
	virtual void OnBeginLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url,
		_In_ bool is_error_page
		)
	{
		(void) caller;
		load_event_args.error_code = 0;
		load_event_args.frame_id = frame_id;
		load_event_args.is_error_page = is_error_page;
		load_event_args.is_main_frame = is_main_frame;
		load_event_args.url = Awesomium::ToString(url.spec());
		view_public->notify.LoadingStarted(view_public, &load_event_args);
	}

	//! This event occurs when a frame fails to load. See error_desc for additional information.
	virtual void OnFailLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url,
		_In_ int error_code,
		_In_ const Awesomium::WebString& error_desc
		)
	{
		(void) caller;
		load_event_args.frame_id = frame_id;
		load_event_args.error_code = error_code;
		load_event_args.is_error_page = FALSE;
		load_event_args.is_main_frame = is_main_frame;
		load_event_args.url = Awesomium::ToString(url.spec());
		view_public->notify.LoadingFailed(view_public, &load_event_args);
	}

	//! This event occurs when the page finishes loading a frame. The main frame always finishes loading last for a given page load.
	virtual void OnFinishLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url
		)
	{
		(void) caller;
		load_event_args.frame_id = frame_id;
		load_event_args.error_code = 0;
		load_event_args.is_error_page = FALSE;
		load_event_args.is_main_frame = is_main_frame;
		load_event_args.url = Awesomium::ToString(url.spec());
		view_public->notify.LoadingFinished(view_public, &load_event_args);
	}

	//! This event occurs when the DOM has finished parsing and the window object is available for JavaScript execution.
	virtual void OnDocumentReady(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		)
	{
		(void) caller;
		load_event_args.url = Awesomium::ToString(url.spec());
		view_public->notify.DocumentBuilt(view_public, &load_event_args);
	}

#pragma endregion // load events

#pragma region Process events

	//! This event occurs when the process hangs.
	virtual void OnUnresponsive(
		_In_ Awesomium::WebView* caller
		)
	{
		process_event_args.responsiveness_state = FALSE;
		view_public->notify.ResponsivenessChanged(view_public, &process_event_args);
	}

	//! This event occurs when the process becomes responsive after a hang.
	virtual void OnResponsive(
		_In_ Awesomium::WebView* caller
		)
	{
		process_event_args.responsiveness_state = TRUE;
		view_public->notify.ResponsivenessChanged(view_public, &process_event_args);
	}

	//! This event occurs when the process crashes.
	virtual void OnCrashed(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::TerminationStatus status
		)
	{
		process_event_args.termination_status = status;
		view_public->notify.ResponsivenessChanged(view_public, &process_event_args);
	}

#pragma endregion // process events

#pragma region Javascript events

	//! This event occurs whenever a custom JSObject method (with no return
	//! value) is called from JavaScript.
	//! @param  caller  The WebView that dispatched this event.
	//! @param  remote_object_id  The unique ID of the JS Object that contains this method.
	//! @param  method_name  The name of the method being called.
	//! @param  args  The arguments passed to the method.
	virtual void OnMethodCall(
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
	virtual Awesomium::JSValue OnMethodCallWithReturnValue(
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

};

#pragma endregion

Nena::InteractiveTV::Web::View::View(Web *core)
	: impl(new Implementation(this))
	, core(core)
{
}

Nena::InteractiveTV::Web::View::~View()
{
	delete impl, impl = nullptr;
}

void Nena::InteractiveTV::Web::View::Initialize(
	Nena::Application::Window *window, 
	Session *session
	)
{
	::BOOL status;
	::DEVMODEA screen;
	::Nena::Platform::ZeroValue(&screen);

	ZeroMemory(&screen, sizeof DEVMODEA);
	status = EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &screen);
	dims.u32[0] = (uint32) screen.dmPelsWidth;
	dims.u32[1] = (uint32) screen.dmPelsHeight;

	impl->Initialize(
		core->impl,		// web core
		dims.u32[0],	// screen width
		dims.u32[1],	// screen height
		session->impl	// session config
		);
}

void Nena::InteractiveTV::Web::View::Release()
{
	impl->Release();
}


void Nena::InteractiveTV::Web::View::LoadURL(_In_ std::string const &url)
{
	impl->view_private->LoadURL(
		Awesomium::WebURL(Awesomium::WSLit(url.c_str()))
		);
}

BOOL Nena::InteractiveTV::Web::View::IsLoading()
{
	return impl->view_private->IsLoading();
}

Nena::InteractiveTV::Web::Session::Session(Web *core)
	: impl(new Implementation(this))
	, core(core)
{
}

Nena::InteractiveTV::Web::Session::~Session()
{
	delete impl, impl = nullptr;
}

void Nena::InteractiveTV::Web::Session::Initialize()
{
	if (impl->session_private) return;

	::UUID uuid;
	::BOOL mkdir_status;
	::RPC_CSTR rpc_cstring;
	::RPC_STATUS rpc_status;
	::CHAR uuid_buffer[MAX_PATH];

	rpc_status = ::UuidCreate(&uuid);
	rpc_status = ::UuidToStringA(&uuid, &rpc_cstring);

	sprintf_s(uuid_buffer, "%s%s/", "./nena-web-session-", rpc_cstring);
	mkdir_status = ::CreateDirectoryA(uuid_buffer, NULL);
	impl->Initialize(core->impl, uuid_buffer);

	::RpcStringFreeA(&rpc_cstring);
}

void Nena::InteractiveTV::Web::Session::Release()
{
	impl->Release();
}

Nena::InteractiveTV::Web::Web() 
	: impl(new Implementation(this))
{
}

Nena::InteractiveTV::Web::~Web()
{
	delete impl, impl = nullptr;
}

void Nena::InteractiveTV::Web::Initialize()
{
	impl->Initialize();
}

void Nena::InteractiveTV::Web::Release()
{
	impl->Release();
}

void Nena::InteractiveTV::Web::Update()
{
	impl->core_private->Update();
}

Nena::InteractiveTV::Web::Session *Nena::InteractiveTV::Web::GetSession()
{
	return &impl->session_public_default;
}

Nena::InteractiveTV::Web::View *Nena::InteractiveTV::Web::GetView()
{
	return &impl->view_public_default;
}

Nena::InteractiveTV::InteractiveTV()
{
	Initialize();
}

Nena::InteractiveTV::~InteractiveTV()
{
	Release();
}

void Nena::InteractiveTV::Initialize()
{
	online.Initialize();
}

void Nena::InteractiveTV::Release()
{
	online.Release();
}