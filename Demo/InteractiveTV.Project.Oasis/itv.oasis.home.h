#include "app.precompiled.h"
#include "itv.oasis.web.h"
#include "itv.oasis.h"

#ifndef __NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__
#define __NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__

namespace InteractiveTV
{
	namespace Project
	{
		struct Oasis::Home
			: public Oasis::State
			, public Oasis::Web::AppBase
			, private Awesomium::JSMethodHandler
			, private Awesomium::WebViewListener::View
			, private Awesomium::WebViewListener::Load
			, private Awesomium::WebViewListener::Process
		{

			Home();
			virtual ~Home();

			void Init();
			void Quit();
			void OnFrameMove();

			::HRESULT CreateDeviceResources();
			::HRESULT CreateDeviceIndependentResources();
			::HRESULT CreateWindowSizeDependentResources();

		private:

			Oasis::Shared *Context;

			::Nena::Graphics::OverlayResources *Overlay;
			::Nena::Graphics::Resources::Direct2DBitmap ViewImage;
			::Nena::Graphics::Resources::Direct2DDrawingStateBlock BlockState;

		public:

#pragma region Oasis State

			virtual void Resume();
			virtual void Suspend();
			virtual void OnStateQuitted(_In_ Oasis::State *);
			virtual void OnStateResumed(_In_ Oasis::State *);
			virtual void OnStateSuspended(_In_ Oasis::State *);
			virtual void OnViewClosed(_In_ Oasis::State::View *);
			virtual void OnViewClosing(_In_ Oasis::State::View *);
			virtual void OnViewToggled(_In_ Oasis::State::View *);
			virtual void OnViewResizedMoved(_In_ Oasis::State::View *);

#pragma endregion

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
	}
}

#endif // !__NENA_INTERACTIVE_TV_OASIS_HOME_INCLUDED__



