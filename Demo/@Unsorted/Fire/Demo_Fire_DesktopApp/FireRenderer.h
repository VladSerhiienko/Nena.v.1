#pragma once
#include "Precompiled.h"
#include "Camera.h"

#define _Decl_prop_w_actions(PropMember, PropType, PropName, Actions) \
	REIGN_HINTINLINING PropType PropName() const { Actions; return PropMember; }

namespace Sandbox
{
	class FireRenderer
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
				) override
			{
				using namespace Sehnsucht;

				const bool writePosition = IsFlagChecked(providedDataMask, Sehnsucht::BasicShapes::ProvidedData::PositionLocal);
				if (writePosition) targetVertex->PositionLocal = Reign::Vector4(positionLocal.x, positionLocal.y, positionLocal.z, 1.0f);
				const bool writeTex = IsFlagChecked(providedDataMask, Sehnsucht::BasicShapes::ProvidedData::TexCoord);
				if (writeTex) targetVertex->TexCoord = Reign::Vector2(texCoord.x, texCoord.y);
			}
		};

	public_shared_access:

		struct Spatial
		{
			Reign::Matrix World;
			Reign::Matrix View;
			Reign::Matrix Proj;
		};

		struct Noise
		{
			float32	FrameTime;
			Reign::Vector3 ScrollSpeeds;
			Reign::Vector3 Scales;
			float32	Reserved0;
		};

		struct Distortion
		{
			Reign::Vector2 Distortion0;
			Reign::Vector2 Distortion1;
			Reign::Vector2 Distortion2;
			Reign::Vector2 ScaleBias;
		};

	public_dtor_access:

		~FireRenderer(_No_args_);

	public_shared_access:

		FireRenderer(_No_args_);

		void Initialize(_No_args_);
		void CreateWindowSizeDependentResources(_No_args_);
		void CreateDeviceIndependentResources(_No_args_);
		void UpdateForWindowSizeChange(_No_args_);
		void CreateDeviceResources(_No_args_);

	public_shared_access:

		_Decl_value_prop_hinl1(m_wicFactory, Rendering::WICImagingFactoryPtr, WICFactory);
		_Decl_value_prop_hinl1(m_d3dContext, Rendering::Context3Ptr, Context3);
		_Decl_value_prop_hinl1(m_d3dDevice, Rendering::Device3Ptr, Device3);

		_Decl_value_prop_finl2(m_fireDistortionData, Distortion, FireDistortion);
		_Decl_value_prop_finl2(m_fireSpatialData, Spatial, FireSpatial);
		_Decl_value_prop_finl2(m_fireNoiseData, Noise, FireNoise);
		_Decl_value_prop_finl2(m_camera, Camera, FireView);

	public_shared_access:

		REIGN_FORCEINLINING bool OnFrameMove(Sehnsucht::BasicTimerViewPtr timer)
		{ // Configure rendering pipeline
			static auto frameTile = 0.00f;
			EditFireNoise()->FrameTime = timer->Elapsed * 0.35f; // frameTile += 0.008f;
			/*EditFireSpatial()->World = Reign::Matrix::CreateRotationX(EditFireNoise()->FrameTime) *
				Reign::Matrix::CreateRotationY(EditFireNoise()->FrameTime);*/

			m_d3dContext->UpdateSubresource(m_fireDistortion.Get(), 0, nullptr, &m_fireDistortionData, 0u, 0u);
			m_d3dContext->UpdateSubresource(m_fireSpatial.Get(), 0, nullptr, &m_fireSpatialData, 0u, 0u);
			m_d3dContext->UpdateSubresource(m_fireNoise.Get(), 0, nullptr, &m_fireNoiseData, 0u, 0u);
			m_fireVertexShaderBufferArray[1] = m_fireNoise.Get();
			m_fireVertexShaderBufferArray[0] = m_fireSpatial.Get();

			return true;
		}

		REIGN_FORCEINLINING bool OnFrame(_No_args_)
		{ // Draw calls
			static const float32 s_factor [] = { 0.0f, 0.0f, 0.0f, 0.0f };
			static const auto s_stride = _Byte_size_uint Vertex;
			static const auto s_sample = 0xffffffff;
			static const auto s_offset = 0u;

			m_d3dContext->IASetIndexBuffer(m_fireIndices.Get(), SEHNSUCHT_DEFAULT16_INDEX_BUFFER_DXGI_FORMAT, s_offset);
			m_d3dContext->IASetVertexBuffers(0, 1, m_fireVertices.GetAddressOf(), &s_stride, &s_offset);
			m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_d3dContext->IASetInputLayout(m_fireInputLayout.Get());

			m_d3dContext->VSSetShader(m_fireVertexShader.Get(), nullptr, 0u);
			m_d3dContext->VSSetConstantBuffers(0, 2, m_fireVertexShaderBufferArray);

			m_d3dContext->PSSetShader(m_firePixelShader.Get(), nullptr, 0u);
			m_d3dContext->PSSetShaderResources(0, 3, m_firePixelShaderTextureArray);
			m_d3dContext->PSSetConstantBuffers(0, 1, m_fireDistortion.GetAddressOf());
			m_d3dContext->PSSetSamplers(0, 2, m_firePixelShaderSamplerStateArray);

			Microsoft::WRL::ComPtr<ID3D11BlendState> blending = nullptr; float32 factor[4]; uint32 sample;
			m_d3dContext->OMGetBlendState(blending.GetAddressOf(), factor, &sample);
			m_d3dContext->OMSetBlendState(m_blendingEnabled.Get(), s_factor, s_sample);

			Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer = nullptr;
			m_d3dContext->RSGetState(rasterizer.GetAddressOf());
			m_d3dContext->RSSetState(m_fireRasterizer.Get());

			m_d3dContext->DrawIndexed(_To_uint m_indices.size(), 0, 0);

			m_d3dContext->OMSetBlendState(blending.Get(), factor, sample);
			m_d3dContext->RSSetState(rasterizer.Get());

			return true;
		}

	protected_shared_access:

		// Required for resource creation and rendering  

		Rendering::WICImagingFactoryPtr m_wicFactory;
		Rendering::RasterizerState m_fireRasterizer;
		Rendering::BlendState m_blendingEnabled;
		Rendering::Context3Ptr m_d3dContext;
		Rendering::Device3Ptr m_d3dDevice;

		/// Vertex Shader Stage

		Rendering::VShader m_fireVertexShader;
		Rendering::VSLayout m_fireInputLayout;
		Rendering::Buffer m_fireVertices;
		Rendering::Buffer m_fireIndices;
		Rendering::Buffer m_fireSpatial;
		Rendering::Buffer m_fireNoise;
		std::vector<Vertex> m_vertices;
		std::vector<uint16> m_indices;
		Spatial m_fireSpatialData;
		Noise m_fireNoiseData;
		Camera m_camera;

		/// Pixel Shader Stage

		Rendering::PShader m_firePixelShader;
		Rendering::SamplerState m_wrapSampler;
		Rendering::SamplerState m_clampSampler;
		Rendering::Buffer m_fireDistortion;
		Distortion m_fireDistortionData;

		///  Resources

		Rendering::SRView m_fireColorTextureView;
		Rendering::SRView m_fireNoiseTextureView;
		Rendering::SRView m_fireBodyTextureView;
		Rendering::Texture2 m_fireColorTexture;
		Rendering::Texture2 m_fireNoiseTexture;
		Rendering::Texture2 m_fireBodyTexture;

		Rendering::SamplerStatePtr m_firePixelShaderSamplerStateArray[2];
		Rendering::SRViewPtr m_firePixelShaderTextureArray[3];
		Rendering::BufferPtr m_fireVertexShaderBufferArray[2];
		bool m_busy;

	};
};