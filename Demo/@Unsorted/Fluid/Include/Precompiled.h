#pragma once

#ifdef REIGN_DESKTOP // Only for desktop apps
#include "HerzeleidPrecompiled.hxx"
#include "HerzeleidWindowSizeChangedEventArgs.hxx"
#include "HerzeleidVisibilityChangedEventArgs.hxx"
#include "HerzeleidWindowActivatedEventArgs.hxx"
#include "HerzeleidCoreWindowEventArgs.hxx"
#include "HerzeleidPointerEventArgs.hxx"
#include "HerzeleidKeyEventArgs.hxx"
#include "HerzeleidCoreWindow.hxx"
#include "HerzeleidCoreDispatcher.hxx"
#include "HerzeleidCoreApplication.hxx"
#include "HerzeleidCoreApplicationView.hxx"
#endif

#include "SehnsuchtPrecompiled.hxx"
#include "SehnsuchtDirectXBase.hxx"
#include "SehnsuchtReaderWriter.hxx"
#include "SehnsuchtLoader.hxx"

namespace Demo
{
#ifdef REIGN_DESKTOP
	typedef HerzeleidAppModelCoreNamespaceImpl::IFrameworkViewSource IProvider; TinyTypeDecl(IProvider); 
	typedef HerzeleidAppModelCoreNamespaceImpl::IFrameworkView IView; TinyTypeDecl(IView); 
	typedef HerzeleidAppModelCoreNamespaceImpl::CoreApplication App; TinyTypeDecl(App); 
	typedef HerzeleidAppModelCoreNamespaceImpl::CoreApplicationView AppView; TinyTypeDecl(AppView); 
	typedef HerzeleidUICoreNamespaceImpl::CoreWindow AppWindow; TinyTypeDecl(AppWindow); 
#else
	typedef Windows::ApplicationModel::Core::IFrameworkViewSource ^IProviderPtr;
	typedef Windows::ApplicationModel::Core::IFrameworkViewSource IProvider;
	typedef Windows::ApplicationModel::Core::IFrameworkView ^IViewPtr;
	typedef Windows::ApplicationModel::Core::IFrameworkView IView;
	typedef Windows::ApplicationModel::Core::CoreApplicationView ^AppViewPtr;
	typedef Windows::ApplicationModel::Core::CoreApplicationView AppView;
	typedef Windows::ApplicationModel::Core::CoreApplication App;
	typedef Windows::UI::Core::CoreWindow ^AppWindowPtr;
	typedef Windows::UI::Core::CoreWindow AppWindow;
#endif
};