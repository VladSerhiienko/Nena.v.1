#include "Assist.h"
#include "DDSLoader.h"
#include "Reader.h"

void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::SamplerStateDescription& samplerDesc)
{
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
}

void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::DepthStencilStateDescription& depthStencilDesc)
{
	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

}

void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::RasterizerStateDescription& rasterDesc)
{
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
}

std::string Nena::Graphics::Resources::Assist::GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

std::wstring Nena::Graphics::Resources::Assist::GetFileExtension(const std::wstring& FileName)
{
	if (FileName.find_last_of(L".") != std::wstring::npos)
		return FileName.substr(FileName.find_last_of(L".") + 1);
	return L"";
}

::BOOL Nena::Graphics::Resources::Assist::IsDDSFile(const std::string& FileName)
{
	std::string extension = GetFileExtension(FileName);
	return std::string("DDS") == extension ||
		std::string("dds") == extension;
}

::BOOL Nena::Graphics::Resources::Assist::IsDDSFile(const std::wstring& FileName)
{
	std::wstring extension = GetFileExtension(FileName);
	return std::wstring(L"DDS") == extension ||
		std::wstring(L"dds") == extension;
}

Nena::Graphics::Resources::Assist::HResultTask  Nena::Graphics::Resources::Assist::LoadTextureAsync(
	_In_ IDirect3DDevice *pDevice,
	_In_ IWICImagingFactory2 *pWICFactory,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ ITexture2 **ppTexture,
	_Outptr_opt_result_nullonfailure_ ISRView **ppSRView
	)
{
	return concurrency::create_task([&]()
	{
		return LoadTexture(
			pDevice, 
			pWICFactory,
			wszFileName, 
			ppTexture,
			ppSRView
			);
	});
}

::HRESULT Nena::Graphics::Resources::Assist::LoadTexture(
	_In_ IDirect3DDevice *pDevice,
	_In_ IWICImagingFactory2 *pWICFactory,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ ITexture2 **ppTexture,
	_Outptr_opt_result_nullonfailure_ ISRView **ppSRView
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Resources::SRView view;
	Resources::Texture2 texture;
	Utility::Reader::Buffer buffer;

	buffer = reader.ReadData(wszFileName);

	if (IsDDSFile(wszFileName))
	{
		Direct3DResource resource;

		result = Utility::CreateDDSTextureFromMemory(
			pDevice, buffer.Data, buffer.Length, 
			resource.GetAddressOf(), &view
			);
		buffer.ReleaseStorage();
		if (FAILED(result)) return result;

		result = resource.As(&texture);
		resource.Reset();

		if (FAILED(result)) return result;
	}
	else
	{
		if (!pWICFactory) return E_POINTER;

		WICStream wicStream;
		result = pWICFactory->CreateStream(&wicStream);

		result = wicStream->InitializeFromMemory(
			buffer.Data, (::DWORD)buffer.Length
			);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		WICBitmapDecoder WicBitmapDecoder;
		result = pWICFactory->CreateDecoderFromStream(
			wicStream.Get(),
			nullptr,
			WICDecodeMetadataCacheOnDemand,
			&WicBitmapDecoder
			);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		WICBitmapFrameDecode wicBitmapFrame;
		result = WicBitmapDecoder->GetFrame(0, &wicBitmapFrame);

		WICFormatConverter wicFormatConverter;
		result = pWICFactory->CreateFormatConverter(&wicFormatConverter);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		result = wicFormatConverter->Initialize(
			wicBitmapFrame.Get(), GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, nullptr, 0.0,
			WICBitmapPaletteTypeCustom
			);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		::UINT32 width, height;
		result = wicBitmapFrame->GetSize(&width, &height);

		std::unique_ptr<byte[]> bitmapPixels(new ::BYTE[width * height * 4]);
		result = wicFormatConverter->CopyPixels(
			nullptr, width * 4, // stride
			width * height * 4, // buffer size
			bitmapPixels.get() // buffer
			);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		SubresourceData initialData;
		ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));
		initialData.pSysMem = bitmapPixels.get();
		initialData.SysMemPitch = width * 4;
		initialData.SysMemSlicePitch = 0;

		CTexture2Description textureDesc(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			width, height,
			1, 1
			);

		result = pDevice->CreateTexture2D(
			&textureDesc,
			&initialData,
			&texture
			);
		if (FAILED(result))
		{
			buffer.ReleaseStorage();
			return result;
		}

		if (ppSRView != nullptr)
		{
			CSRVDescription shaderResourceViewDesc(
				texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D
				);

			result = pDevice->CreateShaderResourceView(
				texture.Get(),
				&shaderResourceViewDesc,
				&view
				);
			if (FAILED(result))
			{
				buffer.ReleaseStorage();
				return result;
			}
		}

		buffer.ReleaseStorage();
	}

	if (ppTexture) (*ppTexture) = texture.Detach();
	if (ppSRView) (*ppSRView) = view.Detach();

	texture = nullptr;
	view = nullptr;

	return S_OK;

}

::HRESULT Nena::Graphics::Resources::Assist::CreateInputLayout(
	_In_ IDirect3DDevice *pDevice,
	_In_reads_bytes_(uBytecodeSize) ::BYTE *pBytecode,
	_In_::UINT32 uBytecodeSize,
	_In_reads_opt_(uLayoutDescNumElements) Resources::VSInputElementDescription pLayoutDescElements [],
	_In_::UINT32 uLayoutDescNumElements,
	_Outptr_opt_result_nullonfailure_ Resources::IVSLayout **ppLayout,
	_In_opt_ std::wstring wszDebugName
	)
{
	::HRESULT result = S_OK;

	if (!pDevice) return E_POINTER;

	if (!pLayoutDescElements)
	{
		// If no input layout is specified, use the BasicVertex layout.
		const VSInputElementDescription layoutDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		result = pDevice->CreateInputLayout(
			layoutDesc, ARRAYSIZE(layoutDesc),
			pBytecode, uBytecodeSize,
			ppLayout
			);
	}
	else
	{
		result = pDevice->CreateInputLayout(
			pLayoutDescElements,
			uLayoutDescNumElements,
			pBytecode,
			uBytecodeSize,
			ppLayout
			);
	}

	if (FAILED(result)) return result;

	if (wszDebugName.size())
	{
		result = SetDebugName(*ppLayout, wszDebugName);
		if (FAILED(result)) return result;
	}

	return result;

}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_In_reads_opt_(layoutDescNumElements) Resources::VSInputElementDescription pLayoutDescElements[],
	_In_::UINT32 uLayoutDescNumElements,
	_Outptr_result_nullonfailure_ Resources::IVShader **ppShader,
	_Outptr_opt_result_nullonfailure_ Resources::IVSLayout **ppLayout
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateVertexShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	if (ppLayout)
	{
		result = CreateInputLayout(
			pDevice, data.Data, (::UINT32)data.Length,
			pLayoutDescElements, uLayoutDescNumElements,
			ppLayout, wszFileName
			);
	}

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_In_reads_opt_(uNumEntries) const Resources::GSSODeclarationEntry streamOutDeclaration[],
	_In_::UINT32 uNumEntries,
	_In_reads_opt_(uNumStrides) const ::UINT32 pBufferStrides[],
	_In_::UINT32 uNumStrides,
	_In_::UINT32 uRasterizedStream,
	_Outptr_result_nullonfailure_ Resources::IGShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateGeometryShaderWithStreamOutput(
		data.Data, data.Length, streamOutDeclaration, uNumEntries, 
		pBufferStrides, uNumStrides, uRasterizedStream, nullptr, 
		ppShader
		);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ Resources::IHShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateHullShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ Resources::IDShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateDomainShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ Resources::IGShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateGeometryShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ Resources::IPShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreatePixelShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}

::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
	_In_ IDirect3DDevice *pDevice,
	_In_ std::wstring wszFileName,
	_Outptr_result_nullonfailure_ Resources::ICShader **ppShader
	)
{
	::HRESULT result;
	Utility::Reader reader;
	Utility::Reader::Buffer data;

	data = reader.ReadData(wszFileName);
	result = pDevice->CreateComputeShader(data.Data, data.Length, nullptr, ppShader);
	if (FAILED(result)) return result;

	result = SetDebugName(*ppShader, wszFileName);
	if (FAILED(result)) return result;

	data.ReleaseStorage();
	return result;
}
