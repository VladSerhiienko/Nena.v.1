#include "app.precompiled.h"
#include "itv.oasis.home.h"
#include "nena.animation.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_HOME_HUB_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_HOME_HUB_INCLUDED__

struct InteractiveTV::Project::Oasis::Home::Hub
	: public InteractiveTV::Project::Oasis::Home::State
	, public InteractiveTV::Project::Oasis::Web::AppBase
	, private Awesomium::JSMethodHandler
	, private Awesomium::WebViewListener::View
	, private Awesomium::WebViewListener::Load
	, private Awesomium::WebViewListener::Process
{
	enum Stage
	{
		kSuspending,
		kSuspended,
		kResuming,
		kResumed,
	};


	Hub(Oasis::Home *);
	~Hub();

	virtual void Init() override;
	virtual void Quit() override;

	virtual void Resume() override;
	virtual void Suspend() override;

	virtual void OnResized() override;
	virtual void OnFrameMove() override;
	virtual void CreateDeviceResources() override;
	virtual void CreateDeviceIndependentResources() override;
	virtual void CreateWindowSizeDependentResources() override;
	virtual void DiscardDeviceResources() override;
	virtual void DiscardWindowSizeDependentResources() override;


	::BOOL IsResizing = FALSE;
	Hub::Stage CurrentStage = kSuspended;
	Oasis::Shared *Context = nullptr;

	::Nena::Graphics::OverlayResources *Overlay = nullptr;
	::Nena::Graphics::Resources::Size *ScreenSize = nullptr;
	::Nena::Graphics::Resources::Direct2DBitmap ViewImage = nullptr;
	::Nena::Graphics::Resources::Direct2DDrawingStateBlock BlockState = nullptr;

public:

	void OnKeyPressed(_In_::UINT32 key);
	void OnKeyReleased(_In_::UINT32 key);
	void OnMouseMoved(_In_::FLOAT x, _In_::FLOAT y);
	void OnMouseLBPressed(_In_::FLOAT x, _In_::FLOAT y);
	void OnMouseRBPressed(_In_::FLOAT x, _In_::FLOAT y);
	void OnMouseLBReleased(_In_::FLOAT x, _In_::FLOAT y);
	void OnMouseRBReleased(_In_::FLOAT x, _In_::FLOAT y);

private:

#pragma region Shortcuts

	::HRESULT OnUpdateViewImage();
	__forceinline Awesomium::WebView *GetView()	{ return Oasis::Web::AppBase::View; }
	__forceinline Awesomium::BitmapSurface *GetViewSurface() { return static_cast<Awesomium::BitmapSurface*>(Oasis::Web::AppBase::View->surface()); }

#pragma endregion

protected:

#pragma region Awesomium WebView Listeners

	virtual void OnChangeTitle(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebString& title
		);
	virtual void OnChangeAddressBar(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		);
	virtual void OnChangeTooltip(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebString& tooltip
		);
	virtual void OnChangeTargetURL(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		);
	virtual void OnChangeCursor(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::Cursor cursor
		);
	virtual void OnChangeFocus(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::FocusedElementType focused_type
		);
	virtual void OnAddConsoleMessage(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebString& message,
		_In_ int line_number,
		_In_ const Awesomium::WebString& source
		);
	virtual void OnShowCreatedWebView(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::WebView* new_view,
		_In_ const Awesomium::WebURL& opener_url,
		_In_ const Awesomium::WebURL& target_url,
		_In_ const Awesomium::Rect& initial_pos,
		_In_ bool is_popup
		);
	virtual void OnBeginLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url,
		_In_ bool is_error_page
		);
	virtual void OnFailLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url,
		_In_ int error_code,
		_In_ const Awesomium::WebString& error_desc
		);
	virtual void OnFinishLoadingFrame(
		_In_ Awesomium::WebView* caller,
		_In_ int64 frame_id,
		_In_ bool is_main_frame,
		_In_ const Awesomium::WebURL& url
		);
	virtual void OnDocumentReady(
		_In_ Awesomium::WebView* caller,
		_In_ const Awesomium::WebURL& url
		);
	virtual void OnUnresponsive(
		_In_ Awesomium::WebView* caller
		);
	virtual void OnResponsive(
		_In_ Awesomium::WebView* caller
		);
	virtual void OnCrashed(
		_In_ Awesomium::WebView* caller,
		_In_ Awesomium::TerminationStatus status
		);
	virtual void OnMethodCall(
		Awesomium::WebView* caller,
		unsigned int remote_object_id,
		const Awesomium::WebString& method_name,
		const Awesomium::JSArray& args
		);
	virtual Awesomium::JSValue OnMethodCallWithReturnValue(
		_In_ Awesomium::WebView* caller,
		_In_ unsigned int remote_object_id,
		_In_ const Awesomium::WebString& method_name,
		_In_ const Awesomium::JSArray& args
		);

#pragma endregion
};

#endif // !__NENA_INTERACTIVE_TV_OASIS_HOME_HUB_INCLUDED__
