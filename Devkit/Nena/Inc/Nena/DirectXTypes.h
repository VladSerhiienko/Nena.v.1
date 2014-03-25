#ifndef __NENA_DIRECTX_TYPES_H__
#define __NENA_DIRECTX_TYPES_H__

#include <wrl.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <wincodec.h>

#include <d2d1_2.h>
#include <d3d11_2.h>
#include <dwrite_2.h>
#include <d2d1_2helper.h>
#include <d2d1effects_1.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#include <Nena\Math.h>

#if _DEBUG || DEBUG
#define __NENA_DIRECTX_Dbg__ 1
#endif

#define __NENA_DIRECTX_WindowsBlue__ 1

//#if NTDDI_VERSION >= NTDDI_WINBLUE
//#define __NENA_DIRECTX_WindowsBlue__ 1
//#ifndef __NENA_DIRECTX_ARMTARGET__
//#include <d3d11_2.h>
//#else
//#include <d3d11_1.h>
//#endif
//#elif NTDDI_VERSION == NTDDI_WIN8
//#define __NENA_DIRECTX_Windows8__ 1
//#include <d3d11_1.h>
//#elif NTDDI_VERSION == NTDDI_WIN7
//#define __NENA_DIRECTX_Windows7__ 1
//#include <d3d11.h>
//#elif NTDDI_VERSION < NTDDI_WIN7
//#warning "Unsupported Windows Version"
//#endif

#define __NENA_ReturnIfFailed(result) if (FAILED(result)) return result;
#define __NENA_ReturnIfFailedByDef __NENA_ReturnIfFailed(result)

namespace Nena
{
	namespace Graphics
	{
		namespace Resources
		{
			typedef Microsoft::WRL::ComPtr<IDXGISwapChain2> DxgiSwapchain;
			typedef Microsoft::WRL::ComPtr<IDXGISwapChain2> Swapchain;
			typedef Microsoft::WRL::ComPtr<IDXGIDevice3> DxgiDevice;
			typedef Microsoft::WRL::ComPtr<IDXGIFactory3> DxgiFactory;
			typedef Microsoft::WRL::ComPtr<IDXGISurface2> DxgiSurface;
			typedef Microsoft::WRL::ComPtr<IDXGIAdapter> DxgiAdapter;
			typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState1> RasterizerState;
			typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext2> Direct3DContext;
			typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext2> Context3;
			typedef Microsoft::WRL::ComPtr<ID3D11Device2> Direct3DDevice;
			typedef Microsoft::WRL::ComPtr<ID3D11Device2> Device3;
			typedef Microsoft::WRL::ComPtr<ID3D11BlendState1> BlendState;
			typedef Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAView;
			typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;
			typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTView;
			typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSView;
			typedef Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
			typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> VSLayout;
			typedef Microsoft::WRL::ComPtr<ID3D11VertexShader> VShader;
			typedef Microsoft::WRL::ComPtr<ID3D11HullShader> HShader;
			typedef Microsoft::WRL::ComPtr<ID3D11DomainShader> DShader;
			typedef Microsoft::WRL::ComPtr<ID3D11GeometryShader> GShader;
			typedef Microsoft::WRL::ComPtr<ID3D11PixelShader> PShader;
			typedef Microsoft::WRL::ComPtr<ID3D11ComputeShader> CShader;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture1D> Direct3DTexture1;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Direct3DTexture2;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture3D> Direct3DTexture3;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2;
			typedef Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3;
			typedef Microsoft::WRL::ComPtr<ID3D11Buffer> Direct3DBuffer;
			typedef Microsoft::WRL::ComPtr<ID3D11Resource> Direct3DResource;
			typedef Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
			typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilState;
			typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencil;
			typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState1> RasterizerState;
			typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState1> Rasterizer;

			typedef RasterizerState::InterfaceType IRasterizerState;
			typedef Rasterizer::InterfaceType IRasterizer;
			typedef DepthStencil::InterfaceType IDepthStencil;
			typedef DepthStencilState::InterfaceType IDepthStencilState;
			typedef Direct3DResource::InterfaceType IDirect3DResource;
			typedef DxgiSwapchain::InterfaceType IDxgiSwapchain;
			typedef DxgiSurface::InterfaceType IDxgiSurface;
			typedef Swapchain::InterfaceType ISwapchain;
			typedef SamplerState::InterfaceType ISamplerState;
			typedef BlendState::InterfaceType IBlendState;
			typedef Direct3DTexture1::InterfaceType IDirect3DTexture1;
			typedef Direct3DTexture2::InterfaceType IDirect3DTexture2;
			typedef Direct3DTexture3::InterfaceType IDirect3DTexture3;
			typedef Texture1::InterfaceType ITexture1;
			typedef Texture2::InterfaceType ITexture2;
			typedef Texture3::InterfaceType ITexture3;
			typedef Direct3DContext::InterfaceType IDirect3DContext;
			typedef Direct3DDevice::InterfaceType IDirect3DDevice;
			typedef Context3::InterfaceType IContext3;
			typedef Device3::InterfaceType IDevice3;
			typedef VSLayout::InterfaceType IVSLayout;
			typedef GShader::InterfaceType IGShader;
			typedef VShader::InterfaceType IVShader;
			typedef PShader::InterfaceType IPShader;
			typedef HShader::InterfaceType IHShader;
			typedef DShader::InterfaceType IDShader;
			typedef CShader::InterfaceType ICShader;
			typedef UAView::InterfaceType IUAView;
			typedef SRView::InterfaceType ISRView;
			typedef Buffer::InterfaceType IBuffer;
			typedef RTView::InterfaceType IRTView;
			typedef DSView::InterfaceType IDSView;

			typedef Microsoft::WRL::ComPtr<ID2D1Factory2> Direct2DFactory;
			typedef Microsoft::WRL::ComPtr<ID2D1Device1> Direct2DDevice;
			typedef Microsoft::WRL::ComPtr<ID2D1DeviceContext1> Direct2DContext;
			typedef Microsoft::WRL::ComPtr<ID2D1Bitmap1> Direct2DBitmap;
			typedef Microsoft::WRL::ComPtr<IDWriteFactory2> DirectWriteFactory;
			typedef Microsoft::WRL::ComPtr<IWICImagingFactory2> WICImagingFactory;
			typedef Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Direct2DSolidColorBrush;
			typedef Microsoft::WRL::ComPtr<ID2D1RadialGradientBrush> Direct2DRadialGradientBrush;
			typedef Microsoft::WRL::ComPtr<ID2D1LinearGradientBrush> Direct2DLinearGradientBrush;
			typedef Microsoft::WRL::ComPtr<ID2D1GradientStopCollection1> Direct2DGradientStopCollection;
			typedef Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock> Direct2DDrawingStateBlock;
			typedef Microsoft::WRL::ComPtr<IDWriteTextLayout> DirectWriteTextLayout;
			typedef Microsoft::WRL::ComPtr<IDWriteTextFormat> DirectWriteTextFormat;
			typedef Microsoft::WRL::ComPtr<IWICBitmapDecoder> WICBitmapDecoder;
			typedef Microsoft::WRL::ComPtr<IWICStream> WICStream;
			typedef Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> WICBitmapFrameDecode;
			typedef Microsoft::WRL::ComPtr<IWICFormatConverter> WICFormatConverter;

			typedef Direct2DDevice::InterfaceType IDirect2DDevice;
			typedef Direct2DContext::InterfaceType IDirect2DContext;
			typedef Direct2DBitmap::InterfaceType IDirect2DBitmap;
			typedef Direct2DFactory::InterfaceType IDirect2DFactory;
			typedef DirectWriteFactory::InterfaceType IDirectWriteFactory;
			typedef WICImagingFactory::InterfaceType IWICImagingFactory;
			typedef DirectWriteTextLayout::InterfaceType IDirectWriteTextLayout;
			typedef DirectWriteTextFormat::InterfaceType IDirectWriteTextFormat;
			typedef Direct2DSolidColorBrush::InterfaceType IDirect2DSolidColorBrush;
			typedef Direct2DRadialGradientBrush::InterfaceType IDirect2DRadialGradientBrush;
			typedef Direct2DLinearGradientBrush::InterfaceType IDirect2DLinearGradientBrush;
			typedef WICFormatConverter::InterfaceType IWICFormatConverter;
			typedef WICBitmapFrameDecode::InterfaceType IWICBitmapFrameDecode;
			typedef WICStream::InterfaceType IWICStream;
			typedef WICBitmapDecoder::InterfaceType IWICBitmapDecoder;

			typedef ::D3D11_TEXTURE1D_DESC Texture1Description;
			typedef ::D3D11_TEXTURE2D_DESC Texture2Description;
			typedef ::D3D11_TEXTURE3D_DESC Texture3Description;
			typedef ::D3D11_SHADER_RESOURCE_VIEW_DESC SRVDescription;
			typedef ::D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDescription;
			typedef ::D3D11_RENDER_TARGET_VIEW_DESC RTVDescription;
			typedef ::D3D11_DEPTH_STENCIL_VIEW_DESC DSVDescription;
			typedef ::D3D11_BUFFER_DESC BufferDescription;
			typedef ::D3D11_BLEND_DESC1 BlendStateDescription;
			typedef ::D3D11_SAMPLER_DESC SamplerStateDescription;

			typedef ::CD3D11_TEXTURE1D_DESC CTexture1Description;
			typedef ::CD3D11_TEXTURE2D_DESC CTexture2Description;
			typedef ::CD3D11_TEXTURE3D_DESC CTexture3Description;
			typedef ::CD3D11_SHADER_RESOURCE_VIEW_DESC CSRVDescription;
			typedef ::CD3D11_UNORDERED_ACCESS_VIEW_DESC CUAVDescription;
			typedef ::CD3D11_RENDER_TARGET_VIEW_DESC CRTVDescription;
			typedef ::CD3D11_DEPTH_STENCIL_VIEW_DESC CDSVDescription;
			typedef ::CD3D11_BUFFER_DESC CBufferDescription;
			typedef ::CD3D11_BLEND_DESC1 CBlendStateDescription;

			typedef ::D3D11_VIEWPORT ViewportDescription;
			typedef ::CD3D11_VIEWPORT CViewportDescription;

			typedef ::D3D11_DEPTH_STENCIL_DESC DepthStencilStateDescription;
			typedef ::D3D11_DEPTH_STENCIL_DESC DepthStencilDescription;
			typedef ::D3D11_RASTERIZER_DESC1 RasterizerStateDescription;
			typedef ::D3D11_RASTERIZER_DESC1 RasterizerDescription;
			typedef ::CD3D11_DEPTH_STENCIL_DESC CDepthStencilStateDescription;
			typedef ::CD3D11_DEPTH_STENCIL_DESC CDepthStencilDescription;
			typedef ::CD3D11_RASTERIZER_DESC1 CRasterizerStateDescription;
			typedef ::CD3D11_RASTERIZER_DESC1 CRasterizerDescription;
			typedef ::CD3D11_SAMPLER_DESC CSampleStateDescription;

			typedef ::D3D11_RENDER_TARGET_BLEND_DESC1 RTBlendDescription;
			typedef ::D3D11_MAPPED_SUBRESOURCE MappedSubresourceData;
			typedef ::D3D11_MAPPED_SUBRESOURCE MappedSubresource;
			typedef ::D3D11_SUBRESOURCE_DATA SubresourceData;
			typedef ::D3D11_SUBRESOURCE_DATA Subresource;
			typedef ::DXGI_SWAP_CHAIN_DESC1 SwapchainDescription;
			typedef ::DXGI_ADAPTER_DESC DxgiAdapterDescription;
			typedef ::DXGI_ADAPTER_DESC AdapterDescription;
			typedef ::D3D11_INPUT_ELEMENT_DESC VSInputElementDescription;
			typedef ::D3D11_SO_DECLARATION_ENTRY GSSODeclarationEntry;
			typedef ::D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology;

			typedef ::D2D1_BITMAP_PROPERTIES1 BitmapProperties;
			typedef ::DWRITE_TEXT_METRICS DirectWriteTextMetrics;

			typedef ::DWRITE_PARAGRAPH_ALIGNMENT DirectWriteParagraphAlignment;
			typedef ::DWRITE_TEXT_ALIGNMENT DirectWriteTextAlignment;
			typedef ::DWRITE_FONT_STRETCH DirectWriteFontStretch;
			typedef ::DWRITE_FONT_WEIGHT DirectWriteFontWeight;
			typedef ::DWRITE_FONT_STYLE DirectWriteFontStyle;
			typedef ::DXGI_MODE_ROTATION DxgiModeRotation;
			typedef ::D3D_FEATURE_LEVEL FeatureLevel;
			typedef ::D3D_DRIVER_TYPE DriverType;
			typedef ::DXGI_FORMAT DxgiFormat;

			typedef ::DWORD DisplayOrientation;
			typedef ::HWND WindowHandle;

			typedef struct RectF
			{
				::FLOAT top, left, bottom, right;
				inline ::FLOAT GetWidth() { return right - left; }
				inline ::FLOAT GetHeight() { return bottom - top; }

				const RectF &operator=(const RectF& other)
				{
					top = other.top;
					bottom = other.bottom;
					left = other.left;
					right = other.right;
					return *this;
				}

				const RectF &operator=(const ::RECT& other)
				{
					top = (::FLOAT)other.top;
					bottom = (::FLOAT)other.bottom;
					left = (::FLOAT)other.left;
					right = (::FLOAT)other.right;
					return *this;
				}

			} Rect;

			typedef struct SizeF
			{
				::FLOAT Width, Height;

				inline SizeF(::FLOAT s = 0) : Width(s), Height(s) { }
				inline SizeF(::FLOAT w, ::FLOAT h) : Width(w), Height(h) { }

				inline ::BOOL operator==(const SizeF& size)
				{
					return Width == size.Width &&
						Height == size.Height;
				}

				inline const SizeF &operator=(const SizeF &other)
				{
					Width = other.Width;
					Height = other.Height;
					return *this;
				}

				inline ::BOOL operator!=(const SizeF& size)
				{
					return Width != size.Width ||
						Height != size.Height;
				}

			} Size;

			struct NullResources
			{
				static IRTView *NullRTV;
				static IDSView *NullDSV;
			};

			template <typename _Ty> inline void ZeroValue(_Ty *pointer, ::size_t size = sizeof _Ty)
			{
				if (pointer) ZeroMemory(pointer, size);
			}

			template <typename _Ty> inline void Free(_Ty *pointer) { if (pointer) { delete pointer; pointer = nullptr; } }
			template <typename _Ty> inline void FreeSequence(_Ty *pointer) { if (pointer) { delete [] pointer; pointer = nullptr; } }

			inline void MapWriteUnmap(
				IDirect3DContext *context,
				IDirect3DResource *buffer, 
				MappedSubresource &subresource, 
				::HANDLE dataToMap,
				::HRESULT &result
				)
			{
				result = context->Map(
					buffer, 0, D3D11_MAP_WRITE_DISCARD, 
					0, &subresource
					);

				if (FAILED(result)) return;
				subresource.pData = (HANDLE) dataToMap;
				context->Unmap(buffer, 0);
			}

			__forceinline void __MapWriteUnmap(
				IDirect3DContext *context,
				IDirect3DResource *buffer,
				MappedSubresource &subresource,
				::HANDLE dataToMap,
				::HRESULT &result
				)
			{
				result = context->Map(
					buffer, 0, D3D11_MAP_WRITE_DISCARD,
					0, &subresource
					);

				if (FAILED(result)) return;
				subresource.pData = (HANDLE) dataToMap;
				context->Unmap(buffer, 0);
			}

		}
	}
}



#endif // !__NENA_DIRECTX_TYPES_H__
