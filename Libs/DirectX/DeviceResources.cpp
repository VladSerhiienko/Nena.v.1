#include "DirectXPch.h"
#include "DeviceResources.h"

const float Nena::Graphics::DeviceResources::DipsPerInch = 96.0f;

::BOOL Nena::Graphics::DeviceResources::UpdateDpi()
{
	::HDC device = 0;
	::INT status = 0;

	device = GetDC(Host);
	Dpi.x = (float)::GetDeviceCaps(device, LOGPIXELSX);
	Dpi.y = (float)::GetDeviceCaps(device, LOGPIXELSY);
	LogicalDpi = Dpi.x;
	status = ::ReleaseDC(Host, device);

	return status;
}

::BOOL Nena::Graphics::DeviceResources::UpdateLogicalSizeAndCurrentOrientation()
{
	::INT status = 0;
	::RECT clientRect = { 0 };
	::DEVMODE screenSettings = { 0 };

	screenSettings.dmSize = sizeof DEVMODE;
	status = ::EnumDisplaySettingsEx(
		nullptr, ENUM_CURRENT_SETTINGS,
		&screenSettings,
		EDS_ROTATEDMODE
		);

	CurrentOrientation = screenSettings.dmDisplayOrientation;

	if (Windowed)
	{
		status = ::GetClientRect(Host, &clientRect);
		LogicalSize.Width = (float) clientRect.right;
		LogicalSize.Height = (float) clientRect.bottom;
	}
	else
	{
		LogicalSize.Width = (float) screenSettings.dmPelsWidth;
		LogicalSize.Height = (float) screenSettings.dmPelsHeight;
	}

	return status;

}

HRESULT Nena::Graphics::DeviceResources::SetWindow(
	_In_ Nena::Graphics::Resources::WindowHandle window
	)
{
	::INT status = 0;

	Host = window;
	NativeOrientation = DMDO_DEFAULT;

	status = UpdateDpi();
	status = UpdateLogicalSizeAndCurrentOrientation();

	return CreateWindowSizeDependentResources();
}

void Nena::Graphics::DeviceResources::RegisterDeviceNotify(
	_In_ Nena::Graphics::IDeviceNotify* deviceNotify
	)
{
	DeviceNotify = deviceNotify;
}

HRESULT Nena::Graphics::DeviceResources::SetLogicalSize(
	_In_ Nena::Graphics::Resources::Size logicalSize
	)
{
	if (LogicalSize != logicalSize)
	{
		LogicalSize = logicalSize;
		return CreateWindowSizeDependentResources();

	} else return S_OK;
}

HRESULT Nena::Graphics::DeviceResources::SetDpi(
	_In_ float dpix,
	_In_ float dpiy
	)
{
	if (Dpi.x != dpix || Dpi.y != dpiy)
	{
		::INT status = 0;
		::RECT clientRect = { 0 };

		Dpi.x = dpix; 
		Dpi.y = dpiy;

		// When the display DPI changes, the logical size of the window (measured in Dips) 
		// also changes and needs to be updated.
		status = UpdateLogicalSizeAndCurrentOrientation();

		return CreateWindowSizeDependentResources();

	} else return S_OK;
}

HRESULT Nena::Graphics::DeviceResources::SetCurrentOrientation(
	_In_ Resources::DisplayOrientation currentOrientation
	)
{
	if (CurrentOrientation != currentOrientation)
	{
		CurrentOrientation = currentOrientation;
		return CreateWindowSizeDependentResources();

	} else return S_OK;
}

Nena::Graphics::DeviceResources::DeviceResources(
	::D3D_DRIVER_TYPE driver,
	::DXGI_USAGE swapchainBuffersUsage,
	::DXGI_FORMAT swapchainBuffersFormat,
	::DXGI_SWAP_EFFECT swapchainEffect,
	::DXGI_ALPHA_MODE swapchainAlphaMode,
	::UINT32 swapchainBufferCount,
	::BOOL supportD2D,
	::BOOL allowFullscreen,
	::BOOL createDeviceResources
	) :	DeviceRenderTargetBase(), 
		RenderTargetUsage(swapchainBuffersUsage),
		RenderTargetFormat(swapchainBuffersFormat),
		SwapchainBufferCount(swapchainBufferCount),
		SwapchainAlphaMode(swapchainAlphaMode),
		SwapchainEffect(swapchainEffect),
		D2DSupport(supportD2D),
		Driver(driver)
{
	SwapchainFlags = allowFullscreen ? 
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 
		(DXGI_SWAP_CHAIN_FLAG) 0;

	::HRESULT error;
	if (createDeviceResources)
	{
		error = CreateDeviceIndependentResources();
		Valid = SUCCEEDED(error); if (FAILED(error)) return;
		error = CreateDeviceResources();
		Valid = SUCCEEDED(error); if (FAILED(error)) return;
	}
}

Nena::Graphics::DeviceResources::~DeviceResources()
{
	Swapchain->SetFullscreenState(FALSE, nullptr);
}

HRESULT Nena::Graphics::DeviceResources::CreateDeviceIndependentResources()
{
	::HRESULT result = S_OK;
	return result;
}

BOOL Nena::Graphics::DeviceResources::SdkLayersAvailable()
{
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
		nullptr,                    // Any feature level will do.
		0,
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
		nullptr,                    // No need to keep the D3D device reference.
		nullptr,                    // No need to know the feature level.
		nullptr                     // No need to keep the D3D device context reference.
		);

	return SUCCEEDED(hr);
}

HRESULT Nena::Graphics::DeviceResources::CreateDeviceResources()
{
	::HRESULT result;

	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D2DSupport ? D3D11_CREATE_DEVICE_BGRA_SUPPORT : 0;

#if defined(_DEBUG)
	if (SdkLayersAvailable())
	{
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create the Direct3D 11 API device object and a corresponding context.
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr,					// Specify nullptr to use the default adapter.
		Driver,						// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Windows Store apps.
		&device,					// Returns the Direct3D device created.
		&Features,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
		);

	if (FAILED(hr))
	{
		// If the initialization fails, fall back to the WARP device.
		result = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			&Features,
			&context
			);

		if (FAILED(result)) 
			return result;
	}

	// Store pointers to the Direct3D 11.1 API device and immediate context.
	result = device.As(&Device);
	if (FAILED(result)) return result;
	result = context.As(&Context);
	if (FAILED(result)) return result;


	return result;
}

HRESULT Nena::Graphics::DeviceResources::ValidateDevice()
{
	using namespace Microsoft::WRL;

	// The D3D Device is no longer valid if the default adapter changed since the device
	// was created or if the device has been removed.

	// First, get the information for the default adapter from when the device was created.

	HRESULT result; 
	
	Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
	result = (Device.As(&dxgiDevice));
	if (FAILED(result)) return result;

	Microsoft::WRL::ComPtr<IDXGIAdapter> deviceAdapter;
	result = (dxgiDevice->GetAdapter(&deviceAdapter));
	if (FAILED(result)) return result;

	Microsoft::WRL::ComPtr<IDXGIFactory2> deviceFactory;
	result = (deviceAdapter->GetParent(IID_PPV_ARGS(&deviceFactory)));
	if (FAILED(result)) return result;

	Microsoft::WRL::ComPtr<IDXGIAdapter1> previousDefaultAdapter;
	result = (deviceFactory->EnumAdapters1(0, &previousDefaultAdapter));
	if (FAILED(result)) return result;

	DXGI_ADAPTER_DESC previousDesc;
	result = (previousDefaultAdapter->GetDesc(&previousDesc));
	if (FAILED(result)) return result;

	// Next, get the information for the current default adapter.

	Microsoft::WRL::ComPtr<IDXGIFactory2> currentFactory;
	result = (CreateDXGIFactory1(IID_PPV_ARGS(&currentFactory)));
	if (FAILED(result)) return result;

	Microsoft::WRL::ComPtr<IDXGIAdapter1> currentDefaultAdapter;
	result = (currentFactory->EnumAdapters1(0, &currentDefaultAdapter));
	if (FAILED(result)) return result;

	DXGI_ADAPTER_DESC currentDesc;
	result = (currentDefaultAdapter->GetDesc(&currentDesc));
	if (FAILED(result)) return result;

	// If the adapter LUIDs don't match, or if the device reports that it has been removed,
	// a new D3D device must be created.

	if (previousDesc.AdapterLuid.LowPart != currentDesc.AdapterLuid.LowPart ||
		previousDesc.AdapterLuid.HighPart != currentDesc.AdapterLuid.HighPart ||
		FAILED(Device->GetDeviceRemovedReason()))
	{
		// Release references to resources related to the old device.

		dxgiDevice = nullptr;
		deviceAdapter = nullptr;
		deviceFactory = nullptr;
		previousDefaultAdapter = nullptr;

		// Create a new device and swap chain.
		result = HandleDeviceLost();
	}

	return result;
}

HRESULT Nena::Graphics::DeviceResources::HandleDeviceLost()
{
	::HRESULT result;
	Swapchain = nullptr;

	if (DeviceNotify) DeviceNotify->OnDeviceLost();

	result = CreateDeviceResources();
	if (FAILED(result)) return result;

	result = CreateWindowSizeDependentResources();
	if (FAILED(result)) return result;

	if (DeviceNotify) DeviceNotify->OnDeviceRestored();

	return S_OK;
}

HRESULT Nena::Graphics::DeviceResources::CreateWindowSizeDependentResources()
{
	OutputDebugString(__TEXT("Nena::Graphics::DeviceResources::CreateWindowSizeDependentResources()\n"));

	::HRESULT result = S_OK;


	if (SwapchainEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
	{
		if (RenderTargetFormat != DXGI_FORMAT_R16G16B16A16_FLOAT &&
			RenderTargetFormat != DXGI_FORMAT_B8G8R8A8_UNORM &&
			RenderTargetFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			SwapchainEffect = DXGI_SWAP_EFFECT_DISCARD;
			OutputDebugStringA(__TEXT("\tSwapchainEffect = DXGI_SWAP_EFFECT_DISCARD\n"));
		}
	}

	DXGI_SCALING scaling = SwapchainEffect != DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL ?
		DXGI_SCALING::DXGI_SCALING_STRETCH :
		DXGI_SCALING::DXGI_SCALING_NONE;

	// Clear the previous window size specific context.
	Resources::IRTView* nullViews[] = { nullptr };
	Context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	RenderTargetView = nullptr;
	DepthStencilView = nullptr;
	Context->Flush();

	// Calculate the necessary render target size in pixels.
	// Prevent zero size DirectX content from being created.
	OutputSize.Width = Dips2Pels(LogicalSize.Width, Dpi.x);
	OutputSize.Height = Dips2Pels(LogicalSize.Height, Dpi.y);
	OutputSize.Width = max(OutputSize.Width, 1);
	OutputSize.Height = max(OutputSize.Height, 1);


	// The width and height of the swap chain must be based on the window's
	// natively-oriented width and height. If the window is not in the native
	// orientation, the dimensions must be reversed.
	Resources::DxgiModeRotation displayRotation = ComputeDisplayRotation();

	bool swapDimensions = displayRotation == DXGI_MODE_ROTATION_ROTATE90 || displayRotation == DXGI_MODE_ROTATION_ROTATE270;
	RenderTargetSize.Width = swapDimensions ? OutputSize.Height : OutputSize.Width;
	RenderTargetSize.Height = swapDimensions ? OutputSize.Width : OutputSize.Height;

	if (Swapchain.Get() != nullptr)
	{
		// If the swap chain already exists, resize it.
		result = Swapchain->ResizeBuffers(
			SwapchainBufferCount,
			static_cast<UINT>(RenderTargetSize.Width), // 0
			static_cast<UINT>(RenderTargetSize.Height), // 0
			RenderTargetFormat,
			SwapchainFlags
			);

		if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET)
		{
			// If the device was removed for any reason, a new device and swap chain will need to be created.
			HandleDeviceLost();

			// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
			// and correctly set up the new device.
			return S_OK;
		}
		else
		{
			if (FAILED(result)) 
				return result;
		}
	}
	else
	{
		// Otherwise, create a new one using the same adapter as the existing Direct3D device.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

		swapChainDesc.Width = static_cast<UINT>(RenderTargetSize.Width); // 0
		swapChainDesc.Height = static_cast<UINT>(RenderTargetSize.Height); // 0
		swapChainDesc.Format = RenderTargetFormat; // This is the most common swap chain format.
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.SampleDesc.Count = 1; 
		swapChainDesc.BufferCount = SwapchainBufferCount;
		swapChainDesc.BufferUsage = RenderTargetUsage;
		swapChainDesc.SwapEffect = SwapchainEffect;
		swapChainDesc.Flags = SwapchainFlags;
		swapChainDesc.AlphaMode = SwapchainAlphaMode;

		swapChainDesc.Scaling = scaling;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFsDesc = { 0 };
		swapChainFsDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFsDesc.RefreshRate.Denominator = 1;
		swapChainFsDesc.RefreshRate.Numerator = 60;
		swapChainFsDesc.Windowed = Windowed;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
		result = Device.As(&dxgiDevice);
		if (FAILED(result)) return result;

		Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
		result = dxgiDevice->GetAdapter(&dxgiAdapter);
		if (FAILED(result)) return result;

		Microsoft::WRL::ComPtr<IDXGIFactory3> dxgiFactory;
		result = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		if (FAILED(result)) return result;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
		result = dxgiFactory->CreateSwapChainForHwnd(
			Device.Get(), Host, 
			&swapChainDesc, &swapChainFsDesc,
			nullptr, swapchain.ReleaseAndGetAddressOf()
			);
		if (FAILED(result)) return result;

		result = swapchain.As(&Swapchain);
		if (FAILED(result)) return result;

		if (Vsync)
		{
			// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
			// ensures that the application will only render after each VSync, minimizing power consumption.
			result = dxgiDevice->SetMaximumFrameLatency(1);
			if (FAILED(result)) return result;
		}
	}

	Swapchain->SetFullscreenState(!Windowed, nullptr);

	// Set the proper orientation for the swap chain, and generate 2D and
	// 3D matrix transformations for rendering to the rotated swap chain.
	// Note the rotation angle for the 2D and 3D transforms are different.
	// This is due to the difference in coordinate spaces.  Additionally,
	// the 3D matrix is specified explicitly to avoid rounding errors.

	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		OrientationXform3D = DirectX::XMLoadFloat4x4(&ScreenRotation::Rotation0);
		break;
	case DXGI_MODE_ROTATION_ROTATE90:
		OrientationXform3D = DirectX::XMLoadFloat4x4(&ScreenRotation::Rotation270);
		break;
	case DXGI_MODE_ROTATION_ROTATE180:
		OrientationXform3D = DirectX::XMLoadFloat4x4(&ScreenRotation::Rotation180);
		break;
	case DXGI_MODE_ROTATION_ROTATE270:
		OrientationXform3D = DirectX::XMLoadFloat4x4(&ScreenRotation::Rotation90);
		break;
	}

	if (SwapchainEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
	{
		result = Swapchain->SetRotation(displayRotation);
		if (FAILED(result)) return result;
	}

	// Create a render target view of the swap chain back buffer.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	result = Swapchain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(result)) return result;

	result = Device->CreateRenderTargetView(
		backBuffer.Get(), nullptr,
		&RenderTargetView
		);
	if (FAILED(result)) return result;

	// Create a depth stencil view for use with 3D rendering if needed.
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		static_cast<UINT>(RenderTargetSize.Width),
		static_cast<UINT>(RenderTargetSize.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
		);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	result = Device->CreateTexture2D(
		&depthStencilDesc, nullptr,
		&depthStencil
		);
	if (FAILED(result)) return result;

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	result =  Device->CreateDepthStencilView(
		depthStencil.Get(), &depthStencilViewDesc,
		&DepthStencilView
		);
	if (FAILED(result)) return result; 
	
	Viewport = CD3D11_VIEWPORT(
		0.0f, 0.0f,
		RenderTargetSize.Width,
		RenderTargetSize.Height
		);

	Context->RSSetViewports(1, &Viewport);

	return result;
}

Nena::Graphics::Resources::DxgiModeRotation Nena::Graphics::DeviceResources::ComputeDisplayRotation()
{
	switch (CurrentOrientation)
	{
	case DMDO_90: return DXGI_MODE_ROTATION_ROTATE90;
	case DMDO_180: return DXGI_MODE_ROTATION_ROTATE180;
	case DMDO_270: return DXGI_MODE_ROTATION_ROTATE270;
	case DMDO_DEFAULT: return DXGI_MODE_ROTATION_IDENTITY;
	} return DXGI_MODE_ROTATION_UNSPECIFIED;
}

HRESULT Nena::Graphics::DeviceResources::Trim()
{
	::HRESULT result;
	Resources::DxgiDevice dxgiDevice;

	result = Device.As(&dxgiDevice);
	if (FAILED(result)) return result;
	else dxgiDevice->Trim();

	return result;
}

float Nena::Graphics::DeviceResources::Dips2Pels(float dips, float dpi)
{
	return floorf(dips * dpi / DipsPerInch + 0.5f); // Round to nearest integer.
}

::HRESULT Nena::Graphics::DeviceResources::Present()
{
	static ::HRESULT result = S_OK;

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	result = Swapchain->Present(Vsync, 0);

	// If the device was removed either by a disconnection or a driver upgrade, we 
	// must recreate all device resources.
	if (result == DXGI_ERROR_DEVICE_REMOVED || 
		result == DXGI_ERROR_DEVICE_RESET)
	{
		return HandleDeviceLost();
	}

	return S_OK;
}
