
#include <Nena\Delegate.h>
#include "RenderTargetBase.h"

#ifndef __NENA_D3DBASE_H__
#define __NENA_D3DBASE_H__

namespace Nena
{
	namespace Graphics
	{
		// Constants used to calculate screen rotations.
		namespace ScreenRotation
		{
			// 0-degree Z-rotation.
			static const DirectX::XMFLOAT4X4 Rotation0(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

			// 90-degree Z-rotation.
			static const DirectX::XMFLOAT4X4 Rotation90(
				0.0f, 1.0f, 0.0f, 0.0f,
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

			// 180-degree Z-rotation.
			static const DirectX::XMFLOAT4X4 Rotation180(
				-1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);

			// 270-degree Z-rotation.
			static const DirectX::XMFLOAT4X4 Rotation270(
				0.0f, -1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
		};

		// The purpose is to notify the host application of the device being lost or created.
		interface IDeviceNotify
		{
			virtual void OnDeviceLost() = 0;
			virtual void OnDeviceRestored() = 0;
		};
		
		struct DeviceResources : // Constrols all the DirectX resources
			public Nena::Graphics::DeviceRenderTargetBase
		{
			typedef ::Nena::Event<void, DeviceResources *> Event;

			// Constructor for DeviceResources.
			Nena::Graphics::DeviceResources::DeviceResources(
				::D3D_DRIVER_TYPE driver = D3D_DRIVER_TYPE_HARDWARE,
				::DXGI_USAGE swapchainBuffersUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT, // ? SHARER_RESOURCES ? UNORDERED_ACCESS
				::DXGI_FORMAT swapchainBuffersFormat = DXGI_FORMAT_B8G8R8A8_UNORM, // ? DXGI_FORMAT_R16G16B16A16_FLOAT
				::DXGI_SWAP_EFFECT swapchainEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, // ? DXGI_SWAP_EFFECT_DISCARD
				::DXGI_ALPHA_MODE swapchainAlphaMode = DXGI_ALPHA_MODE_IGNORE,
				::UINT32 swapchainBufferCount = 2,
				::BOOL supportD2D = TRUE,
				::BOOL allowFullscreen = TRUE,
				::BOOL createDeviceResources = TRUE
				);

			// Destructor for DeviceResources.
			Nena::Graphics::DeviceResources::~DeviceResources();

			// This method is called when the CoreWindow is created (or re-created).
			::HRESULT Nena::Graphics::DeviceResources::SetWindow(
				_In_ Nena::Graphics::Resources::WindowHandle window
				);

			// Register our DeviceNotify to be informed on device lost and creation.
			void Nena::Graphics::DeviceResources::RegisterDeviceNotify(
				_In_ Nena::Graphics::IDeviceNotify* deviceNotify
				);

			// This method is called in the event handler for the SizeChanged event.
			::HRESULT Nena::Graphics::DeviceResources::SetLogicalSize(
				_In_ Nena::Graphics::Resources::Size size
				);

			// When the display DPI changes, the logical size of the window (measured in Dips) also changes and needs to be updated.
			::HRESULT Nena::Graphics::DeviceResources::SetDpi(
				_In_ float dpix,
				_In_ float dpiy
				);

			// Recreate all device resources and set them back to the current state.
			::HRESULT Nena::Graphics::DeviceResources::HandleDeviceLost();

			// This method is called in the event handler for the OrientationChanged event.
			::HRESULT Nena::Graphics::DeviceResources::SetCurrentOrientation(
				_In_ Resources::DisplayOrientation currentOrientation
				);

			// Call this method when the app suspends. 
			// It provides a hint to the driver that the app is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
			::HRESULT Nena::Graphics::DeviceResources::Trim();

			// This method is called in the event handler  for the DisplayContentsInvalidated event.
			::HRESULT Nena::Graphics::DeviceResources::ValidateDevice();

			// These resources need to be recreated every time the window size is changed.
			::HRESULT Nena::Graphics::DeviceResources::CreateWindowSizeDependentResources();

			// Configures resources that don't depend on the Direct3D device.
			::HRESULT Nena::Graphics::DeviceResources::CreateDeviceIndependentResources();

			// Configures the Direct3D device, and stores handles to it and the device context.
			::HRESULT Nena::Graphics::DeviceResources::CreateDeviceResources();

			// Present the contents of the swap chain to the screen.
			::HRESULT Nena::Graphics::DeviceResources::Present();

			// Check for SDK Layer support.
			::BOOL Nena::Graphics::DeviceResources::SdkLayersAvailable();

			// This method determines the rotation between the display device's native Orientation and the current display orientation.
			Resources::DxgiModeRotation Nena::Graphics::DeviceResources::ComputeDisplayRotation();

			::BOOL Nena::Graphics::DeviceResources::UpdateLogicalSizeAndCurrentOrientation();
			::BOOL Nena::Graphics::DeviceResources::UpdateDpi();

			// Determines device-independent pixels (DIPs) count per 1 inch.
			static const ::FLOAT Nena::Graphics::DeviceResources::DipsPerInch;

			// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
			static ::FLOAT Nena::Graphics::DeviceResources::Dips2Pels(::FLOAT dips, ::FLOAT dpi);

			DeviceResources::Event DeviceLost;			// Release device resources
			DeviceResources::Event DeviceRestored;		// Create device resources
			DeviceResources::Event SwapchainResizing;	// Release window size dependent resources
			DeviceResources::Event SwapchainResized;	// Create window size dependent resources

			// Direct3D objects.
			::Nena::Graphics::Resources::Context3 Context;
			::Nena::Graphics::Resources::Device3 Device;
			::Nena::Graphics::Resources::Swapchain Swapchain;

			::Nena::Graphics::Resources::DriverType Driver;
			::Nena::Graphics::Resources::WindowHandle Host;
			::Nena::Graphics::IDeviceNotify *DeviceNotify;

			::Nena::Graphics::Resources::FeatureLevel Features;
			::Nena::Graphics::Resources::Size RenderTargetSize;
			::Nena::Graphics::Resources::Size ActualRenderTargetSize;
			::Nena::Graphics::Resources::Size LogicalSize;
			::Nena::Graphics::Resources::Size OutputSize;

			::DXGI_SWAP_CHAIN_FLAG SwapchainFlags;
			::DXGI_ALPHA_MODE SwapchainAlphaMode;
			::DXGI_SWAP_EFFECT SwapchainEffect;
			::DXGI_FORMAT RenderTargetFormat;
			::DXGI_USAGE RenderTargetUsage;
			::UINT32 SwapchainBufferCount;
			::DirectX::XMFLOAT2 Dpi;
			::FLOAT LogicalDpi;

			::BOOL D2DSupport;
			::BOOL Windowed;
			::BOOL Vsync;
			::BOOL Valid;

			::DirectX::XMMATRIX OrientationXform3D;
			Nena::Graphics::Resources::DisplayOrientation NativeOrientation;
			Nena::Graphics::Resources::DisplayOrientation CurrentOrientation;

		};
	}
}

#endif // !__NENA_D3DBASE_H__
