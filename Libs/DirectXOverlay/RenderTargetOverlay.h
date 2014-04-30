
#ifndef __NENA_DIRECTX_OVERLAY_H__
#define __NENA_DIRECTX_OVERLAY_H__

#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>

namespace Nena
{
	namespace Graphics
	{

		// Should be used as an addition to DeviceResources for 2D rendering
		// and should be managed using DeviceResources.
		struct OverlayResources
		{
			Nena::Graphics::OverlayResources::OverlayResources();
			Nena::Graphics::OverlayResources::~OverlayResources();

			// Call *before* DeviceResources::SetDpi() or DeviceResources::CreateWindowSizeDependentResources.
			void Nena::Graphics::OverlayResources::SetDpi(
				_In_ Nena::Graphics::DeviceResources *resources,
				_In_::FLOAT x, _In_::FLOAT y
				);

			// Call *after* DeviceResources::CreateDeviceIndependentResources().
			::HRESULT Nena::Graphics::OverlayResources::CreateDeviceIndependentResources(
				);
			::HRESULT Nena::Graphics::OverlayResources::CreateDeviceResources(
				_In_ Nena::Graphics::DeviceResources *resources
				);

			// Call *after* DeviceResources::CreateWindowSizeDependentResources() to update target bitmap.
			::HRESULT Nena::Graphics::OverlayResources::CreateWindowSizeDependentResources(
				_In_ Nena::Graphics::DeviceResources *resources
				);

			// Calculates 2D orientation matrix for direct 2D context.
			void Nena::Graphics::OverlayResources::SetDisplayRotation(
				_In_::DXGI_MODE_ROTATION displayRotation,
				_In_::FLOAT logicalWidth,
				_In_::FLOAT logicalHeight
				);

			// Direct2D drawing components.
			Nena::Graphics::Resources::Direct2DFactory D2DFactory;
			Nena::Graphics::Resources::Direct2DDevice Device;
			Nena::Graphics::Resources::Direct2DContext Context;
			Nena::Graphics::Resources::Direct2DBitmap TargetBitmap;

			// DirectWrite drawing components.
			Nena::Graphics::Resources::DirectWriteFactory DWFactory;
			Nena::Graphics::Resources::WICImagingFactory WICFactory;

			// Configuration.
			::D2D1::Matrix3x2F OrientationXform2D;
			::DirectX::XMFLOAT2 Dpi;
			::BOOL Valid;

		};
	}
}


#endif // !__NENA_DIRECTX_OVERLAY_H__
