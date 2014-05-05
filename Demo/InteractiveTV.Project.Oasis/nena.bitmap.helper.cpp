#include "app.precompiled.h"
#include "nena.bitmap.helper.h"

Nena::Graphics::Resources::Direct2DBitmap Nena::Graphics::Resources::BitmapHelp::LoadFromFile(
	_In_ BitmapHelp::String8 uri_utf8, _Out_opt_::HRESULT *result
	)
{
	BitmapHelp::String16 uri( uri_utf8.begin( ), uri_utf8.end( ) );
	return LoadFromFile( uri, result );
}

Nena::Graphics::Resources::Direct2DBitmap Nena::Graphics::Resources::BitmapHelp::LoadFromFile(
	_In_ BitmapHelp::String uri, _Out_opt_::HRESULT *result
	)
{
	using namespace InteractiveTV;

	Nena::Graphics::Resources::Direct2DBitmap bitmap = nullptr;
	auto overlay = &Oasis::GetForCurrentThread( )->Context.Engine->Graphics.d2d;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder = NULL;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pSource = NULL;
	Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter = NULL;

	HRESULT hr = overlay->WICFactory->CreateDecoderFromFilename(
		uri.c_str( ), NULL, GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		pDecoder.ReleaseAndGetAddressOf( )
		);

	if ( SUCCEEDED( hr ) )
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame( 0, &pSource );
	}

	if ( SUCCEEDED( hr ) )
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = overlay->WICFactory->CreateFormatConverter( &pConverter );
	}

	if ( SUCCEEDED( hr ) )
	{
		hr = pConverter->Initialize(
			pSource.Get( ),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}

	if ( SUCCEEDED( hr ) )
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = overlay->Context->CreateBitmapFromWicBitmap(
			pConverter.Get( ), NULL,
			bitmap.ReleaseAndGetAddressOf( )
			);
	}

	if ( result ) (*result) = hr;
	return bitmap;
}