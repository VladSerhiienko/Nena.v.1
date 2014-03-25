#pragma once

#include "pch.h"
#include "BasicReaderWriter.h"
#include "DirectXHelper.h"

// A simple loader class that provides support for loading shaders, textures,
// and meshes from files on disk. Provides synchronous and asynchronous methods.
ref class BasicLoader
{
internal:

	template <class TBufferContent>
	inline static void CreateVertexBuffer(
		_In_ ID3D11Device1 *pDevice,
		_In_ uint32 uNumBufferElements,
		_Outref_result_nullonfailure_ ID3D11Buffer **ppBuffer,
		_Const_ TBufferContent const *pInitialData
		)
	{
		D3D11_BUFFER_DESC BufferDescriptor = { 0 };
		BufferDescriptor.ByteWidth = uNumBufferElements * GetByteSizeOf(TBufferContent);
		BufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA BufferInitData = { 0 };
		BufferInitData.pSysMem = pInitialData;

		DX::ThrowIfFailed(
			//"FluidSimulator: Failed to create vertex buffer.", 
			pDevice->CreateBuffer( 
				&BufferDescriptor, 
				&BufferInitData, 
				ppBuffer 
				)
			);
	}



	template <class TBufferContent>
	inline static void CreateConstantBuffer(
		_In_ ID3D11Device1 *pDevice,
		_Outref_result_nullonfailure_ ID3D11Buffer **ppBuffer,
		_Const_ _Maybenull_ TBufferContent const *pInitialData = nullptr
		)
	{
		D3D11_BUFFER_DESC BufferDescriptor = { 0 };
		BufferDescriptor.ByteWidth = GetByteSizeOf(TBufferContent);
		BufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

		if ( pInitialData )
		{
			D3D11_SUBRESOURCE_DATA BufferInitData = { 0 };
			BufferInitData.pSysMem = pInitialData;

			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create constant buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					&BufferInitData, 
					ppBuffer 
					)
				);
		}
		else
		{
			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create constant buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					nullptr, 
					ppBuffer 
					)
				);
		}
	}

	template <class TBufferContent>
	inline static void CreateStructuredBuffer(
		_In_ ID3D11Device1 *pDevice,
		_In_ uint32 uNumBufferElements,
		_Outref_result_nullonfailure_ ID3D11Buffer **ppBuffer,
		_Outref_result_nullonfailure_ ID3D11ShaderResourceView **ppSRView,
		_Outref_result_nullonfailure_ ID3D11UnorderedAccessView **ppUAView,
		_Const_ _Maybenull_  TBufferContent const *pInitialData = nullptr
		)
	{
		D3D11_BUFFER_DESC BufferDescriptor;
		Reign::Standard::ZeroValue( &BufferDescriptor );
		BufferDescriptor.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		BufferDescriptor.ByteWidth = uNumBufferElements * GetByteSizeOf(TBufferContent);
		BufferDescriptor.StructureByteStride = GetByteSizeOf(TBufferContent);
		BufferDescriptor.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		BufferDescriptor.Usage = D3D11_USAGE_DEFAULT;

		if ( pInitialData )
		{

			D3D11_SUBRESOURCE_DATA BufferInitData;
			Reign::Standard::ZeroValue( &BufferInitData );
			BufferInitData.pSysMem = pInitialData;

			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					&BufferInitData, 
					ppBuffer 
					)
				);
		}
		else
		{
			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer.", 
				pDevice->CreateBuffer( 
					&BufferDescriptor, 
					nullptr, 
					ppBuffer 
					)
				);
		}

		if ( ppSRView ) 
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC BufferSRViewDesc;
			Reign::Standard::ZeroValue( &BufferSRViewDesc );
			BufferSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			BufferSRViewDesc.Buffer.ElementWidth = uNumBufferElements;

			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer srv.", 
				pDevice->CreateShaderResourceView( 
					(*ppBuffer), (&BufferSRViewDesc), 
					(ppSRView)
					)
				);
		}

		if ( ppUAView )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC BufferUAViewDesc;
			Reign::Standard::ZeroValue( &BufferUAViewDesc );
			BufferUAViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			BufferUAViewDesc.Buffer.NumElements = uNumBufferElements;
		
			DX::ThrowIfFailed(
				//"FluidSimulator: Failed to create structured buffer uav.", 
				pDevice->CreateUnorderedAccessView( 
					(*ppBuffer), (&BufferUAViewDesc), 
					(ppUAView)
					)
				);
		}
	}

    BasicLoader(
        _In_ ID3D11Device* d3dDevice,
        _In_opt_ IWICImagingFactory2* wicFactory = nullptr
        );

    void LoadTexture(
        _In_ Platform::String^ filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    concurrency::task<void> LoadTextureAsync(
        _In_ Platform::String^ filename,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC layoutDesc[],
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11VertexShader** shader,
        _Out_opt_ ID3D11InputLayout** layout
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11PixelShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11PixelShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11ComputeShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11ComputeShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11GeometryShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32 numEntries,
        _In_reads_opt_(numStrides) const uint32* bufferStrides,
        _In_ uint32 numStrides,
        _In_ uint32 rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _In_reads_opt_(numEntries) const D3D11_SO_DECLARATION_ENTRY* streamOutDeclaration,
        _In_ uint32 numEntries,
        _In_reads_opt_(numStrides) const uint32* bufferStrides,
        _In_ uint32 numStrides,
        _In_ uint32 rasterizedStream,
        _Out_ ID3D11GeometryShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11HullShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11HullShader** shader
        );

    void LoadShader(
        _In_ Platform::String^ filename,
        _Out_ ID3D11DomainShader** shader
        );

    concurrency::task<void> LoadShaderAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11DomainShader** shader
        );

    void LoadMesh(
        _In_ Platform::String^ filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount
        );

    concurrency::task<void> LoadMeshAsync(
        _In_ Platform::String^ filename,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount
        );

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<IWICImagingFactory2> m_wicFactory;
    BasicReaderWriter^ m_basicReaderWriter;

    template <class DeviceChildType>
    inline void SetResourceDebugName(
        _In_ DeviceChildType* object,
        _In_ Platform::String^ name
        );

    Platform::String^ GetExtension(
        _In_ Platform::String^ filename
        );

    void CreateTexture(
        _In_ bool decodeAsDDS,
        _In_reads_bytes_(dataSize) byte* data,
        _In_ uint32 dataSize,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ ID3D11ShaderResourceView** textureView,
        _In_opt_ Platform::String^ debugName
        );

    void CreateInputLayout(
        _In_reads_bytes_(bytecodeSize) byte* bytecode,
        _In_ uint32 bytecodeSize,
        _In_reads_opt_(layoutDescNumElements) D3D11_INPUT_ELEMENT_DESC* layoutDesc,
        _In_ uint32 layoutDescNumElements,
        _Out_ ID3D11InputLayout** layout
        );

    void CreateMesh(
        _In_ byte* meshData,
        _Out_ ID3D11Buffer** vertexBuffer,
        _Out_ ID3D11Buffer** indexBuffer,
        _Out_opt_ uint32* vertexCount,
        _Out_opt_ uint32* indexCount,
        _In_opt_ Platform::String^ debugName
        );
};
