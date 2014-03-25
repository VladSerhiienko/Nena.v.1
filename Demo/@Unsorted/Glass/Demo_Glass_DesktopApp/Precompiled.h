#pragma once

#ifdef REIGN_DESKTOP // Only for desktop apps
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

#include "SehnsuchtDirectXBaseDesktop.hxx"
#include "SehnsuchtBasicRenderTarget.hxx"
#include "SehnsuchtBasicShapes.hxx"
#include "SehnsuchtLoader.hxx"
#include "SehnsuchtTimer.hxx"
#include "ReignMath.hxx"
#include "ReignCore.hxx"

namespace Sandbox
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

	namespace Rendering
	{

#if _Reign_desktop_support_windows_7
		typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerState;
		typedef Microsoft::WRL::ComPtr<IWICImagingFactory> WICImagingFactory;
		typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context3;
		typedef Microsoft::WRL::ComPtr<ID3D11BlendState> BlendState;
		typedef Microsoft::WRL::ComPtr<ID3D11Device> Device3;
		typedef D3D11_RASTERIZER_DESC RasterizerStateDescrition;
		typedef D3D11_BLEND_DESC BlendStateDescrition;
#else
		typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState1> RasterizerState;
		typedef Microsoft::WRL::ComPtr<IWICImagingFactory2> WICImagingFactory;
		typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Context3;
		typedef Microsoft::WRL::ComPtr<ID3D11BlendState1> BlendState;
		typedef Microsoft::WRL::ComPtr<ID3D11Device1> Device3;
		typedef D3D11_RASTERIZER_DESC1 RasterizerStateDescrition;
		typedef D3D11_BLEND_DESC1 BlendStateDescrition;
#endif
		typedef Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAView;
		typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;
		typedef Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
		typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;
		typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSView;
		typedef Microsoft::WRL::ComPtr<ID3D11GeometryShader> GShader;
		typedef Microsoft::WRL::ComPtr<ID3D11VertexShader> VShader;
		typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> VSLayout;
		typedef Microsoft::WRL::ComPtr<ID3D11PixelShader> PShader;
		typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2;
		typedef Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;

		typedef WICImagingFactory::InterfaceType *WICImagingFactoryPtr;
		typedef SamplerState::InterfaceType *SamplerStatePtr;
		typedef BlendState::InterfaceType *BlendStatePtr;
		typedef Texture2::InterfaceType *Texture2Ptr;
		typedef Context3::InterfaceType *Context3Ptr;
		typedef Device3::InterfaceType *Device3Ptr;
		typedef VSLayout::InterfaceType *VSLayoutPtr;
		typedef GShader::InterfaceType *GShaderPtr;
		typedef VShader::InterfaceType *VShaderPtr;
		typedef PShader::InterfaceType *PShaderPtr;
		typedef UAView::InterfaceType *UAViewPtr;
		typedef SRView::InterfaceType *SRViewPtr;
		typedef Buffer::InterfaceType *BufferPtr;
		typedef RTView::InterfaceType *RTViewPtr;
		typedef DSView::InterfaceType *DSViewPtr;
	};
};