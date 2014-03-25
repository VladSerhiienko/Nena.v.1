#pragma once
#include "Precompiled.h"
#include "Renderer.h"

namespace Sandbox
{
	ref_class_decl View : public Sandbox::IView
	{
#ifdef REIGN_DESKTOP // Desktop
		typedef HerzeleidFoundationNamespaceImpl::TypedEventHandler<Sandbox::AppWindow, HerzeleidUICoreNamespaceImpl::VisibilityChangedEventArgs> AppWindowVisiblityEventHandler;
		typedef HerzeleidFoundationNamespaceImpl::TypedEventHandler<Sandbox::AppWindow, HerzeleidUICoreNamespaceImpl::WindowActivatedEventArgs> AppWindowActivationEventHandler;
		typedef HerzeleidFoundationNamespaceImpl::TypedEventHandler<Sandbox::AppWindow, HerzeleidUICoreNamespaceImpl::WindowSizeChangedEventArgs> AppWindowSizingEventHandler;
		typedef HerzeleidFoundationNamespaceImpl::TypedEventHandler<Sandbox::AppWindow, HerzeleidUICoreNamespaceImpl::PointerEventArgs> AppWindowPointerEventHandler;
		typedef HerzeleidFoundationNamespaceImpl::TypedEventHandler<Sandbox::AppWindow, HerzeleidUICoreNamespaceImpl::CoreWindowEventArgs> AppWindowEventHandler;
		typedef HerzeleidUICoreNamespaceImpl::VisibilityChangedEventArgs AppWindowVisibilityEventArgs; TinyTypeDecl( AppWindowVisibilityEventArgs );
		typedef HerzeleidUICoreNamespaceImpl::WindowActivatedEventArgs AppWindowActivationEventArgs; TinyTypeDecl( AppWindowActivationEventArgs );
		typedef HerzeleidUICoreNamespaceImpl::WindowSizeChangedEventArgs AppWindowSizingEventArgs; TinyTypeDecl( AppWindowSizingEventArgs );
		typedef HerzeleidUICoreNamespaceImpl::PointerEventArgs AppWindowPointerEventArgs; TinyTypeDecl( AppWindowPointerEventArgs );
		typedef HerzeleidUICoreNamespaceImpl::CoreWindowEventArgs AppWindowEventArgs; TinyTypeDecl( AppWindowEventArgs );
		typedef HerzeleidUICoreNamespaceImpl::CoreProcessEventsOption AppWindowProcessEventsOption; TinyTypeDecl( AppWindowProcessEventsOption );
		typedef HerzeleidUICoreNamespaceImpl::CoreWindowActivationState AppWindowActivationState;
		typedef String16ConstRef EntryPointString;
#else // Runtime
		typedef Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::VisibilityChangedEventArgs^> AppWindowVisiblityEventHandler;
		typedef Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowActivatedEventArgs^> AppWindowActivationEventHandler;
		typedef Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::WindowSizeChangedEventArgs^> AppWindowSizingEventHandler;
		typedef Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^> AppWindowPointerEventHandler;
		typedef Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::CoreWindowEventArgs^> AppWindowEventHandler;
		typedef Windows::UI::Core::VisibilityChangedEventArgs ^AppWindowVisibilityEventArgsPtr;
		typedef Windows::UI::Core::VisibilityChangedEventArgs AppWindowVisibilityEventArgs;
		typedef Windows::UI::Core::WindowActivatedEventArgs ^AppWindowActivationEventArgsPtr;
		typedef Windows::UI::Core::WindowActivatedEventArgs AppWindowActivationEventArgs;
		typedef Windows::UI::Core::VisibilityChangedEventArgs ^AppWindowVisibilityEventArgsPtr;
		typedef Windows::UI::Core::VisibilityChangedEventArgs AppWindowVisibilityEventArgs;
		typedef Windows::UI::Core::WindowSizeChangedEventArgs ^AppWindowSizingEventArgsPtr;
		typedef Windows::UI::Core::WindowSizeChangedEventArgs AppWindowSizingEventArgs;
		typedef Windows::UI::Core::PointerEventArgs ^AppWindowPointerEventArgsPtr;
		typedef Windows::UI::Core::PointerEventArgs AppWindowPointerEventArgs;
		typedef Windows::UI::Core::CoreWindowEventArgs ^AppWindowEventArgsPtr;
		typedef Windows::UI::Core::CoreWindowEventArgs AppWindowEventArgs;
		typedef Windows::UI::Core::CoreProcessEventsOption AppWindowProcessEventsOption;
		typedef Windows::UI::Core::CoreWindowActivationState AppWindowActivationState;
		typedef Platform::String^ EntryPointString;
#endif // All

	public:

		virtual void Initialize( _In_ Sandbox::AppViewPtr ApplicationView ) override;
		virtual void Load( _In_ Sandbox::View::EntryPointString wszEntryPoint ) override;
		virtual void SetWindow( _In_ Sandbox::AppWindowPtr Window ) override;
		virtual void Uninitialize( ) override;
		virtual void Run( ) override;

		void OnPointerMoved( 
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
			);
		void OnPointerPressed( 
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
			);
		void OnPointerReleased( 
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowPointerEventArgsPtr pArgs
			);

		void OnWindowSizeChanged(
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowSizingEventArgsPtr pArgs
			);
		void OnWindowActivatedChanged(
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowActivationEventArgsPtr pArgs
			);
		void OnWindowVisibilityChanged(
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowVisibilityEventArgsPtr pArgs
			);
		void OnWindowClosed(
			_In_ Sandbox::AppWindowPtr pWindow,
			_In_ Sandbox::View::AppWindowEventArgsPtr pArgs
			);

	private:

		Reign::Standard::Bool m_bActivated;
		Reign::Standard::Bool m_bVisible;
		Reign::Standard::Bool m_bClosed;

		Renderer *m_renderer;

	};

	ref_class_decl Provider : public Sandbox::IProvider
	{
	public:

		virtual Sandbox::IViewPtr CreateView( ) override;
	};
};

