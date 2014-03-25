#ifndef __NENA_DIRECTX_RESOURCES_ASSIST_H__
#define __NENA_DIRECTX_RESOURCES_ASSIST_H__

#include <Nena\DirectXTypes.h>
#include <ppltasks.h>

namespace Nena
{
	namespace Graphics
	{
		namespace Resources
		{
			template <class TDirectXResource> struct ReadResource
			{
				TDirectXResource Rsrc;
				Resources::SRView Read;
			};

			template <class TDirectXResource> struct WriteResource
			{
				TDirectXResource Rsrc;
				Resources::UAView Write;
			};

			template <class TDirectXResource> struct ActiveResource
			{
				TDirectXResource Rsrc;
				Resources::SRView Read;
				Resources::UAView Write;
			};

			template <class TDirectXResource> struct PingPongResource
			{
				ActiveResource<TDirectXResource> Ping;
				ActiveResource<TDirectXResource> Pong;

				inline void PingPong()
				{
					Ping.Read.Swap(Pong.Read);
					Ping.Write.Swap(Pong.Write);
				}
			};

			struct Assist
			{
				typedef concurrency::task<::HRESULT> HResultTask;

				// just sets default values to desc provided / use before setting your own configuration
				static void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::DepthStencilStateDescription& desc);

				// just sets default values to desc provided / use before setting your own configuration
				// determines how and what polygons will be drawn.
				static void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::RasterizerStateDescription& desc);
				static void Nena::Graphics::Resources::Assist::Defaults(Nena::Graphics::Resources::SamplerStateDescription& desc);

				static std::wstring Nena::Graphics::Resources::Assist::GetFileExtension(const std::wstring& FileName);
				static std::string Nena::Graphics::Resources::Assist::GetFileExtension(const std::string& FileName);
				static ::BOOL Nena::Graphics::Resources::Assist::IsDDSFile(const std::wstring& FileName);
				static ::BOOL Nena::Graphics::Resources::Assist::IsDDSFile(const std::string& FileName);

				static ::HRESULT Nena::Graphics::Resources::Assist::LoadTexture(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ Resources::IWICImagingFactory *pWICFactory,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ ITexture2 **ppTexture,
					_Outptr_opt_result_nullonfailure_ ISRView **ppSRView
					);

				static HResultTask Nena::Graphics::Resources::Assist::LoadTextureAsync(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ Resources::IWICImagingFactory *pWICFactory,
					_In_ std::wstring wszFileName, 
					_Outptr_result_nullonfailure_ ITexture2 **ppTexture,
					_Outptr_opt_result_nullonfailure_ ISRView **ppSRView
					);

				static ::HRESULT Nena::Graphics::Resources::Assist::CreateInputLayout(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_reads_bytes_(uBytecodeSize) ::BYTE *pBytecode,
					_In_ ::UINT32 uBytecodeSize,
					_In_reads_opt_(uLayoutDescNumElements) Resources::VSInputElementDescription pLayoutDescElements[],
					_In_::UINT32 uLayoutDescNumElements,
					_Outptr_opt_result_nullonfailure_ Resources::IVSLayout **ppLayout,
					_In_opt_ std::wstring wszDebugName = L""
					);

				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_In_reads_opt_(layoutDescNumElements) Resources::VSInputElementDescription pLayoutDescElements[],
					_In_ ::UINT32 uLayoutDescNumElements,
					_Outptr_result_nullonfailure_ Resources::IVShader **ppShader,
					_Outptr_opt_result_nullonfailure_ Resources::IVSLayout **ppLayout
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_In_reads_opt_(uNumEntries) const Resources::GSSODeclarationEntry streamOutDeclaration[],
					_In_::UINT32 uNumEntries,
					_In_reads_opt_(uNumStrides) const ::UINT32 pBufferStrides[],
					_In_::UINT32 uNumStrides,
					_In_::UINT32 uRasterizedStream,
					_Outptr_result_nullonfailure_ Resources::IGShader **ppShader
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ Resources::IHShader **ppShader
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ Resources::IDShader **ppShader
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ Resources::IGShader **ppShader
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ Resources::IPShader **ppShader
					);
				static ::HRESULT Nena::Graphics::Resources::Assist::LoadShader(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ std::wstring wszFileName,
					_Outptr_result_nullonfailure_ Resources::ICShader **ppShader
					);

				template <class TTarget>
				static void *ZeroValue(TTarget *t, ::UINT32 s = sizeof TTarget)
				{
					return ZeroMemory(t, s);
				}

				/// Templates (Assist.inl)

				template <class TTarget> 
				static ::HRESULT SetDebugName(
					_In_ TTarget *target,
					_In_ std::string name
					);
				template <class TTarget> 
				static ::HRESULT SetDebugName(
					_In_ TTarget *target,
					_In_ std::wstring name
					);

				template <class TBufferContent>
				static ::HRESULT CreateVertexBuffer(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_::UINT32 uNumBufferElements,
					_Const_ TBufferContent const *pInitialData,
					_Outref_result_nullonfailure_ IBuffer **ppBuffer
					);

				template <typename TIndex = ::UINT16>
				static ::HRESULT CreateIndexBuffer(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_ ::UINT32 uNumIndices,
					_Const_ TIndex const *pIndexData,
					_Outref_result_nullonfailure_ Resources::IBuffer **ppIndexBuffer
					);

				template <class TBufferContent>
				static ::HRESULT CreateConstantBuffer(
					_In_ Resources::IDirect3DDevice *pDevice,
					_Const_ _Maybenull_ TBufferContent const *pInitialData,
					_Outref_result_nullonfailure_ IBuffer **ppBuffer
					);

				template <class TBufferContent>
				static ::HRESULT CreateStructuredBuffer(
					_In_ Resources::IDirect3DDevice *pDevice,
					_In_::UINT32 uNumBufferElements,
					_Const_ TBufferContent const *pInitialData,
					_Outref_result_nullonfailure_ IBuffer **ppBuffer,
					_Outref_result_nullonfailure_ ISRView **ppSRView,
					_Outref_result_nullonfailure_ IUAView **ppUAView
					);
			};

#include "Assist.inl"


			// Strongly typed wrapper around a D3D constant buffer.
			template<typename T>
			class ConstantBuffer
			{
			public:
				ConstantBuffer() {}

				::HRESULT Create(_In_ IDirect3DDevice* device)
				{
					::HRESULT result = S_OK;
					Resources::BufferDescription desc = { 0 };

					desc.ByteWidth = sizeof(T);
					desc.Usage = D3D11_USAGE_DYNAMIC;
					desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

					result = device->CreateBuffer(&desc, nullptr, m_buffer.ReleaseAndGetAddressOf());
					if (SUCCEEDED(result)) Assist::SetDebugName(m_buffer.Get(), "NenaDirectXResources");

					return result;
				}


				::HRESULT SetData(_In_ IDirect3DContext* deviceContext, T const& value)
				{
					::HRESULT result = S_OK;
					if (!m_buffer.Get()) return E_FAIL;

					Resources::MappedSubresource mappedResource;

					result = deviceContext->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

					if (SUCCEEDED(result))
					{
						*(T*) mappedResource.pData = value;
						deviceContext->Unmap(m_buffer.Get(), 0);
					}

					return result;
				}

				IBuffer* GetBuffer()
				{
					return m_buffer.Get();
				}

				IBuffer **GetBufferAddress()
				{
					return m_buffer.GetAddressOf();
				}

			private:

				Resources::Buffer m_buffer;

				ConstantBuffer(ConstantBuffer const&);
				ConstantBuffer& operator= (ConstantBuffer const&);
			};

		}
	}
}

#endif // !__NENA_DIRECTX_RESOURCES_ASSIST_H__
