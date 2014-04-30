#include "RenderTargetOverlay.h"

Nena::Graphics::OverlayResources::OverlayResources() : Dpi(96.0f, 96.0f)
{
	SetDisplayRotation(DXGI_MODE_ROTATION_IDENTITY, 800, 600);
	Valid = SUCCEEDED(CreateDeviceIndependentResources());
}

Nena::Graphics::OverlayResources::~OverlayResources()
{
	WICFactory.ReleaseAndGetAddressOf();
}

void Nena::Graphics::OverlayResources::SetDisplayRotation(
	_In_::DXGI_MODE_ROTATION displayRotation,
	_In_::FLOAT logicalWidth,
	_In_::FLOAT logicalHeight
	)
{
	switch (displayRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		OrientationXform2D = D2D1::Matrix3x2F::Identity();
		break;
	case DXGI_MODE_ROTATION_ROTATE90:
		OrientationXform2D =
			D2D1::Matrix3x2F::Rotation(90.0f) *
			D2D1::Matrix3x2F::Translation(logicalHeight, 0.0f);
		break;
	case DXGI_MODE_ROTATION_ROTATE180:
		OrientationXform2D =
			D2D1::Matrix3x2F::Rotation(180.0f) *
			D2D1::Matrix3x2F::Translation(logicalWidth, logicalHeight);
		break;
	case DXGI_MODE_ROTATION_ROTATE270:
		OrientationXform2D =
			D2D1::Matrix3x2F::Rotation(270.0f) *
			D2D1::Matrix3x2F::Translation(0.0f, logicalWidth);
		break;
	}
}


::HRESULT Nena::Graphics::OverlayResources::CreateDeviceResources(
	_In_ Nena::Graphics::DeviceResources *resources
	)
{
	::HRESULT result = S_OK;

	auto device = resources->Device.Get();
	Resources::DxgiDevice dxgiDevice;

	result = device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	if (FAILED(result)) return result;

	result = D2DFactory->CreateDevice(dxgiDevice.Get(), &Device);
	if (FAILED(result)) return result;

	result = Device->CreateDeviceContext(
		D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
		//D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&Context
		);
	return result;
}

::HRESULT Nena::Graphics::OverlayResources::CreateDeviceIndependentResources()
{
	::HRESULT result = S_OK;

	// Initialize Direct2D resources.
	::D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Initialize the Direct2D Factory.
	result = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory2),
		&options,
		&D2DFactory
		);
	if (FAILED(result)) return result;

	// Initialize the DirectWrite Factory.
	result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory2),
		&DWFactory
		);
	if (FAILED(result)) return result;

	// Initialize the Windows Imaging Component (WIC) Factory.
	result = CoCreateInstance(
		CLSID_WICImagingFactory2,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&WICFactory)
		);
	return result;
}

::HRESULT Nena::Graphics::OverlayResources::CreateWindowSizeDependentResources(
	_In_ Nena::Graphics::DeviceResources *resources
	)
{
	::HRESULT result = S_OK;

	auto swapchain = resources->Swapchain.Get();

	DXGI_SWAP_CHAIN_DESC1 surfaceDesc;
	result = swapchain->GetDesc1(&surfaceDesc);

	// Create a Direct2D target bitmap associated with the
	// swap chain back buffer and set it as the current target.
	Resources::BitmapProperties bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(surfaceDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED),
		Dpi.x,
		Dpi.y
		);

	Resources::DxgiSurface dxgiBackBuffer;
	result = swapchain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
	if (FAILED(result)) return result;

	result = Context->CreateBitmapFromDxgiSurface(
		dxgiBackBuffer.Get(),
		&bitmapProperties,
		&TargetBitmap
		);
	if (FAILED(result)) return result;

	Context->SetTarget(TargetBitmap.Get());

	// Grayscale text anti-aliasing is recommended for all Windows Store apps.
	Context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	return result;
}


void Nena::Graphics::OverlayResources::SetDpi(
	_In_ Nena::Graphics::DeviceResources *resources,
	_In_::FLOAT x, _In_::FLOAT y
	)
{
	Context->SetDpi(x, y);
	CreateWindowSizeDependentResources(resources);
}



