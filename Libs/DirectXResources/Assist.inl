
template <class TTarget> static ::HRESULT Nena::Graphics::Resources::Assist::SetDebugName(
	_In_ TTarget *target,
	_In_ std::wstring name
	)
{
	return SetDebugName(target, std::string(name.begin(), name.end()));
}

template <class TTarget> static ::HRESULT Nena::Graphics::Resources::Assist::SetDebugName(
	_In_ TTarget *target,
	_In_ std::string name
	)
{
	if (target) return target->SetPrivateData(
		WKPDID_D3DDebugObjectName,
		(::UINT32) name.size(),
		(::HANDLE) name.c_str()
		);
	else return E_POINTER;
}

template <typename TIndex>
::HRESULT Nena::Graphics::Resources::Assist::CreateIndexBuffer(
	_In_ Resources::IDirect3DDevice *pDevice,
	_In_::UINT32 uNumIndices,
	_Const_ TIndex const *pIndexData,
	_Outref_result_nullonfailure_ Resources::IBuffer **ppIndexBuffer
	)
{
	::HRESULT result = S_OK;
	
	Resources::BufferDescription indexBufferDesc = { 0 };
	indexBufferDesc.ByteWidth = uNumIndices * sizeof TIndex;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	Resources::SubresourceData indexBufferData = { 0 };
	indexBufferData.pSysMem = pIndexData;

	return pDevice->CreateBuffer(
		&indexBufferDesc, &indexBufferData,
		ppIndexBuffer
		);
}

template <class TBufferContent>
::HRESULT Nena::Graphics::Resources::Assist::CreateVertexBuffer(
	_In_ Nena::Graphics::Resources::IDirect3DDevice *pDevice,
	_In_::UINT32 uNumBufferElements,
	_Const_ TBufferContent const *pInitialData,
	_Outref_result_nullonfailure_ Nena::Graphics::Resources::IBuffer **ppBuffer
	)
{
	BufferDescription BufferDescriptor = { 0 };
	BufferDescriptor.ByteWidth = uNumBufferElements * sizeof (TBufferContent);
	BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
	//BufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

	SubresourceData BufferInitData = { 0 };
	BufferInitData.pSysMem = pInitialData;

	return pDevice->CreateBuffer(
		&BufferDescriptor,
		&BufferInitData,
		ppBuffer
		);
}

template <class TBufferContent>
::HRESULT Nena::Graphics::Resources::Assist::CreateConstantBuffer(
	_In_ Nena::Graphics::Resources::IDirect3DDevice *pDevice,
	_Const_ _Maybenull_ TBufferContent const *pInitialData,
	_Outref_result_nullonfailure_ Nena::Graphics::Resources::IBuffer **ppBuffer
	)
{
	BufferDescription BufferDescriptor = { 0 };
	BufferDescriptor.ByteWidth = sizeof (TBufferContent);
	//BufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
	//BufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;

	if (pInitialData)
	{
		SubresourceData BufferInitData = { 0 };
		BufferInitData.pSysMem = pInitialData;

		return pDevice->CreateBuffer(
			&BufferDescriptor,
			&BufferInitData,
			ppBuffer
			);
	}
	else
	{
		return pDevice->CreateBuffer(
			&BufferDescriptor,
			nullptr,
			ppBuffer
			);
	}
}

template <class TBufferContent>
::HRESULT Nena::Graphics::Resources::Assist::CreateStructuredBuffer(
	_In_ Nena::Graphics::Resources::IDirect3DDevice *pDevice,
	_In_::UINT32 uNumBufferElements,
	_Const_ _Maybenull_  TBufferContent const *pInitialData,
	_Outref_result_nullonfailure_ IBuffer **ppBuffer,
	_Outref_result_nullonfailure_ ISRView **ppSRView,
	_Outref_result_nullonfailure_ IUAView **ppUAView
	)
{
	::HRESULT result = S_OK;

	BufferDescription BufferDescriptor = { 0 };
	BufferDescriptor.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	BufferDescriptor.ByteWidth = uNumBufferElements * sizeof (TBufferContent);
	BufferDescriptor.StructureByteStride = sizeof (TBufferContent);
	BufferDescriptor.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

	if (pInitialData)
	{
		SubresourceData BufferInitData = { 0 };
		BufferInitData.pSysMem = pInitialData;

		result = pDevice->CreateBuffer(
			&BufferDescriptor,
			&BufferInitData,
			ppBuffer
			);
	}
	else
	{
		result = pDevice->CreateBuffer(
			&BufferDescriptor,
			nullptr,
			ppBuffer
			);
	}

	if (ppSRView)
	{
		SRVDescription BufferSRViewDesc; ZeroValue(&BufferSRViewDesc);
		BufferSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		BufferSRViewDesc.Buffer.ElementWidth = uNumBufferElements;

		result = pDevice->CreateShaderResourceView(
			*ppBuffer, &BufferSRViewDesc,
			ppSRView
			);
	}

	if (ppUAView)
	{
		UAVDescription BufferUAViewDesc; ZeroValue(&BufferUAViewDesc);
		BufferUAViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		BufferUAViewDesc.Buffer.NumElements = uNumBufferElements;

		result = pDevice->CreateUnorderedAccessView(
			*ppBuffer, &BufferUAViewDesc,
			ppUAView
			);
	}

	return result;
}