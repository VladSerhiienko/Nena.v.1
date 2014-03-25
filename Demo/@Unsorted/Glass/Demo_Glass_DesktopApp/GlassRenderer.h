#pragma once
#include "Precompiled.h"
#include "RendererUtil.h"

namespace Sandbox
{
	class GlassRenderer
	{
	public_shared_access:

		struct Vertex
		{
			Reign::Vector4 PositionLocal;
			Reign::Vector2 TexCoord;
		};

		struct Converter : public Sehnsucht::BasicShapes::IVertexDataProvider<Vertex>
		{
			virtual void FillVertex(
				_In_ Reign::Vector3 const &positionLocal, // Vertex position in the model space.
				_In_ Reign::Vector3 const &normalLocal, // Vertex normal in the model space.
				_In_ Reign::Vector2 const &texCoord, // Vertex texture coordinates.
				_In_ Sehnsucht::BasicShapes::ProvidedData providedDataMask, // Specifies what data is actually provided.
				_Inout_ Vertex *targetVertex // Vertex to fill the data in.
				) override;
		};

	public_shared_access:

		_Reign_apply_dx_cb_alignment struct Spatial
		{
			Reign::Matrix World;
			Reign::Matrix View;
			Reign::Matrix Proj;
		};


		_Reign_apply_dx_cb_alignment struct Glass
		{
			float32 RefractionPerturb;
		};

	public_dtor_access :

		~GlassRenderer(_No_args_);

	public_shared_access:

		GlassRenderer(_No_args_);

		void Initialize(_No_args_);
		void CreateWindowSizeDependentResources(_No_args_);
		void CreateDeviceIndependentResources(_No_args_);
		void CreateDeviceResources(_No_args_);

		void UpdateForWindowSizeChange(_In_ Sandbox::EventArgsForWindowSizeChange args);

	public_shared_access:

		_Decl_value_prop_hinl1(m_sceneRenderTarget, Sehnsucht::BasicRenderTarget*, SceneRenderTarget); // Get/Set WIC Imaging Factory
		_Decl_value_prop_hinl1(m_wicFactory, Rendering::WICImagingFactoryPtr, WICFactory); // Get/Set WIC Imaging Factory
		_Decl_value_prop_hinl1(m_d3dContext, Rendering::Context3Ptr, Context3); // Get/Set D3D 11 Device Context
		_Decl_value_prop_hinl1(m_d3dDevice, Rendering::Device3Ptr, Device3); // Get/Set D3D 11 Device
		_Decl_value_prop_finl2(m_glassSpatialData, Spatial, GlassSpatial); // Get/Edit/Set Glass Spatial Data
		_Decl_value_prop_finl2(m_glassPerturbData, Glass, GlassPerturb); // Get/Edit/Set Glass Perturb Data

		REIGN_FORCEINLINING bool OnFrameMove(Sehnsucht::BasicTimerViewPtr timer)
		{
			// Configure rendering pipeline
			m_d3dContext->UpdateSubresource(m_glassSpatial.Get(), 0, nullptr, &m_glassSpatialData, 0u, 0u);
			m_d3dContext->UpdateSubresource(m_glassPerturb.Get(), 0, nullptr, &m_glassPerturbData, 0u, 0u);
			return true;
		}

		REIGN_FORCEINLINING bool OnFrame(_No_args_)
		{
			static ID3D11ShaderResourceView *s_null_srvs[3] = { nullptr };
			static const float32 s_factor [] = { 0.0f, 0.0f, 0.0f, 0.0f };
			static const auto s_stride = _Byte_size_uint Vertex;
			static const auto s_sample = 0xffffffff;
			static const auto s_offset = 0u;

			m_d3dContext->IASetIndexBuffer(m_glassIndices.Get(), SEHNSUCHT_DEFAULT16_INDEX_BUFFER_DXGI_FORMAT, s_offset);
			m_d3dContext->IASetVertexBuffers(0, 1, m_glassVertices.GetAddressOf(), &s_stride, &s_offset);
			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_d3dContext->IASetInputLayout(m_glassLayout.Get());

			m_d3dContext->VSSetShader(m_glassVertexShader.Get(), nullptr, 0u);
			m_d3dContext->VSSetConstantBuffers(0, 1, m_glassSpatial.GetAddressOf());

			m_d3dContext->PSSetShader(m_glassPixelShader.Get(), nullptr, 0u);
			m_d3dContext->PSSetConstantBuffers(0, 1, m_glassPerturb.GetAddressOf());
			m_d3dContext->PSSetSamplers(0, 1, m_glassLinearSampler.GetAddressOf());
			m_d3dContext->PSSetShaderResources(0, 3, m_glassPixelShaderTextureViews);

			m_d3dContext->DrawIndexed(_To_uint m_glassIndicesData.size(), 0, 0);

			m_d3dContext->PSSetShaderResources(0, 3, s_null_srvs);
			return true;
		}

	protected_shared_access:

		// Required for resource creation and rendering  

		Rendering::WICImagingFactoryPtr m_wicFactory;
		Rendering::Context3Ptr m_d3dContext;
		Rendering::Device3Ptr m_d3dDevice;

		/// Rasterizer/Merger

		Rendering::RasterizerState m_glassRasterizer;
		Rendering::BlendState m_glassBlendingEnabled;

		/// Vertex Shader Stage

		Rendering::VShader m_glassVertexShader;
		Rendering::VSLayout m_glassLayout;
		Rendering::Buffer m_glassSpatial;
		Rendering::Buffer m_glassVertices;
		Rendering::Buffer m_glassIndices;

		std::vector<Vertex> m_glassVerticesData;
		std::vector<uint16> m_glassIndicesData;
		Spatial m_glassSpatialData;

		/// Pixel Shader Stage

		Rendering::PShader m_glassPixelShader;
		Rendering::Buffer m_glassPerturb;
		Rendering::SamplerState m_glassLinearSampler;
		Rendering::Texture2 m_glassNormalTexture;
		Rendering::Texture2 m_glassColorTexture;
		Rendering::SRView m_glassNormalTextureView;
		Rendering::SRView m_glassColorTextureView;
		Glass m_glassPerturbData;

		Rendering::SRViewPtr m_glassPixelShaderTextureViews[3];
		Sehnsucht::BasicRenderTarget *m_sceneRenderTarget;

	};
};
