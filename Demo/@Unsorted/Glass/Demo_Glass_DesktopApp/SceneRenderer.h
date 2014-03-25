#pragma once
#include "Precompiled.h"
#include "RendererUtil.h"

namespace Sandbox
{
	class SceneRenderer
	{
	public_shared_access:

		struct Vertex
		{
			Reign::Vector4 PositionLocal;
			Reign::Vector3 NormalLocal;
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
				) override
			{
				using namespace Sehnsucht;

				const bool writePosition = IsFlagChecked(providedDataMask, Sehnsucht::BasicShapes::ProvidedData::PositionLocal);
				if (writePosition) targetVertex->PositionLocal = Reign::Vector4(positionLocal.x, positionLocal.y, positionLocal.z, 1.0f);
				const bool writeNormal = IsFlagChecked(providedDataMask, Sehnsucht::BasicShapes::ProvidedData::NormalLocal);
				if (writeNormal) targetVertex->NormalLocal = Reign::Vector3(normalLocal.x, normalLocal.y, normalLocal.z);
				const bool writeTex = IsFlagChecked(providedDataMask, Sehnsucht::BasicShapes::ProvidedData::TexCoord);
				if (writeTex) targetVertex->TexCoord = Reign::Vector2(texCoord.x, texCoord.y);
			}
		};

		_Reign_apply_dx_cb_alignment struct Spatial
		{
			Reign::Matrix World;
			Reign::Matrix View;
			Reign::Matrix Proj;
		};

	public_dtor_access:

		~SceneRenderer(_No_args_);

	public_shared_access:

		SceneRenderer(_No_args_);

		void Initialize(_No_args_);
		void CreateWindowSizeDependentResources(_No_args_);
		void CreateDeviceIndependentResources(_No_args_);
		void CreateDeviceResources(_No_args_);

		void UpdateForWindowSizeChange(_In_ Sandbox::EventArgsForWindowSizeChange args);

		REIGN_FORCEINLINING bool OnFrameMove(Sehnsucht::BasicTimerViewPtr timer)
		{
			// Configure rendering pipeline
			m_d3dContext->UpdateSubresource(
				m_sceneSpatial.Get(), 0, nullptr, 
				&m_sceneSpatialData, 0u, 0u
				);

			return true;
		}

		REIGN_FORCEINLINING bool OnFrame(_No_args_)
		{
			static const float32 s_factor [] = { 0.0f, 0.0f, 0.0f, 0.0f };
			static const auto s_stride = _Byte_size_uint Vertex;
			static const auto s_sample = 0xffffffff;
			static const auto s_offset = 0u;

			m_d3dContext->IASetIndexBuffer(m_cubeIndices.Get(), SEHNSUCHT_DEFAULT16_INDEX_BUFFER_DXGI_FORMAT, s_offset);
			m_d3dContext->IASetVertexBuffers(0, 1, m_cubeVertices.GetAddressOf(), &s_stride, &s_offset);
			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_d3dContext->IASetInputLayout(m_sceneLayout.Get());

			m_d3dContext->VSSetShader(m_sceneVertexShader.Get(), nullptr, 0u);
			m_d3dContext->VSSetConstantBuffers(0, 1, m_sceneSpatial.GetAddressOf());

			m_d3dContext->PSSetShader(m_scenePixelShader.Get(), nullptr, 0u);
			m_d3dContext->PSSetShaderResources(0, 1, m_cubeColorTextureView.GetAddressOf());
			m_d3dContext->PSSetSamplers(0, 1, m_sceneSampler.GetAddressOf());

			m_d3dContext->DrawIndexed(_To_uint m_cubeIndicesData.size(), 0, 0);
			return true;
		}

	public_shared_access:

		_Decl_value_prop_hinl1(m_wicFactory, Rendering::WICImagingFactoryPtr, WICFactory); // Get/Set WIC Imaging Factory
		_Decl_value_prop_hinl1(m_d3dContext, Rendering::Context3Ptr, Context3); // Get/Set D3D 11 Device Context
		_Decl_value_prop_hinl1(m_d3dDevice, Rendering::Device3Ptr, Device3); // Get/Set D3D 11 Device
		_Decl_value_prop_finl2(m_sceneSpatialData, Spatial, SceneSpatial);

	protected_shared_access:

		// Required for resource creation and rendering  

		Rendering::WICImagingFactoryPtr m_wicFactory;
		Rendering::Context3Ptr m_d3dContext;
		Rendering::Device3Ptr m_d3dDevice;

		// Pipeline

		Rendering::VShader m_sceneVertexShader;
		Rendering::PShader m_scenePixelShader;
		Rendering::VSLayout m_sceneLayout;
		Rendering::Buffer m_sceneSpatial;
		Rendering::SamplerState m_sceneSampler;
		Spatial m_sceneSpatialData;

		// Scene objects
		// Cube

		Rendering::SRView m_cubeColorTextureView;
		Rendering::Texture2 m_cubeColorTexture;
		std::vector<Vertex> m_cubeVerticesData;
		std::vector<uint16> m_cubeIndicesData;
		Rendering::Buffer m_cubeVertices;
		Rendering::Buffer m_cubeIndices;


	};
}