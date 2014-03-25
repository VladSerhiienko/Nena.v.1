#pragma once

#include "pch.h"

ref class ShadowRenderer
{
#pragma region Timer
private: typedef Application::Utility::Timing::TimerView TimerView;
#pragma endregion

private:

	REIGN_ALIGN(16)
	struct Spatial
	{
		Reign::Matrix g_World;
		Reign::Matrix g_CameraView;
		Reign::Matrix g_CameraProj;
		Reign::Matrix g_LightView[4];
		Reign::Matrix g_LightProj[4];
	} m_spatialData; // vs

	REIGN_ALIGN(16)
	struct LightSpatial
	{
		//uint32 g_NumSparkles[4];
		Reign::Vector4 g_LightPositionWorld[4];
	} m_lightSpatialData; // vs

	REIGN_ALIGN(16)
	struct NumLights
	{
		uint32 g_NumSparkles;
	} m_numLightsData; // vs

	REIGN_ALIGN(16)
	struct LightColors
	{
		Reign::Vector4 g_AmbientColor;
		Reign::Vector4 g_DiffuseColor[4];
	} m_lightColorsData; // vs

	boolean m_updateLightSpatial; // vs
	boolean m_updateLightColors; // vs
	boolean m_updateSpatial; // vs
	uint32 m_numIndices;

	// From the external source

	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice; // d3d
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext; // d3d

	// Internally created

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_clampSampler; // ps
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_wrapSampler; // ps
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowVertexShader; // vs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shadowInputLayout; // vs il
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shadowPixelShader; // ps
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightSpatial; // vs
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightColors; // ps
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spatial; // vs
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_numLights; // vs

	// Convinient way to config vertex shader stage const buffers.
	ID3D11Buffer *m_shadowVertexShaderBufferArray[3]; // vs
	// Convinient way to config pixel shader stage samplers.
	ID3D11SamplerState *m_shadowPixelShaderSamplerArray[2]; // ps
	// Convinient way to config pixel shader stage texture views.
	ID3D11ShaderResourceView *m_shadowPixelShaderTextureViewArray[5]; // ps

internal:

	REIGN_FORCEINLINING LightSpatial const *GetLightSpatial() const { return &m_lightSpatialData; }
	REIGN_FORCEINLINING LightSpatial GetLightSpatialCpy() const { return m_lightSpatialData; }
	REIGN_FORCEINLINING LightSpatial *EditLightSpatial() { return &m_lightSpatialData; }

	REIGN_FORCEINLINING NumLights const *GetNumLights() const { return &m_numLightsData; }
	REIGN_FORCEINLINING NumLights GetNumLightsCpy() const { return m_numLightsData; }
	REIGN_FORCEINLINING NumLights *EditNumLights() { return &m_numLightsData; }

	REIGN_FORCEINLINING LightColors const *GetLightColors() const { return &m_lightColorsData; }
	REIGN_FORCEINLINING LightColors GetLightColorsCpy() const { return m_lightColorsData; }
	REIGN_FORCEINLINING LightColors *EditLightColors() { m_updateLightColors = true; return &m_lightColorsData; }

	REIGN_FORCEINLINING Spatial const *GetSpatial() const { return &m_spatialData; }
	REIGN_FORCEINLINING Spatial GetSpatialCpy() const { return m_spatialData; }
	REIGN_FORCEINLINING Spatial *EditSpatial() { return &m_spatialData; }

	REIGN_HINTINLINING uint32 GetNumIndices( ) const { return m_numIndices; }
	REIGN_HINTINLINING void SetNumIndices( _In_ uint32 numIndices ) { m_numIndices = numIndices; }

	REIGN_FORCEINLINING void OnFrame() 
	{ 
		m_d3dContext->VSSetConstantBuffers(0, 3, m_shadowVertexShaderBufferArray); // cb
		m_d3dContext->PSSetShaderResources(0, 5, m_shadowPixelShaderTextureViewArray); // t

		/*if (m_updateSpatial)*/ 
		m_d3dContext->UpdateSubresource(
			m_spatial.Get(), 0, nullptr, 
			(&m_spatialData), 0, 0
			);
		/*if (m_updateLightSpatial)*/ 
		m_d3dContext->UpdateSubresource(
			m_lightSpatial.Get(), 0, nullptr, 
			(&m_lightSpatialData), 0, 0
			);
		/*if (m_updateLightColors)*/ 
		m_d3dContext->UpdateSubresource(
			m_lightColors.Get(), 0, nullptr, 
			(&m_lightColorsData), 0, 0
			);
		m_d3dContext->UpdateSubresource(
			m_numLights.Get(), 0, nullptr, 
			(&m_numLightsData), 0, 0
			);
		m_d3dContext->DrawIndexed(
			m_numIndices, 0, 0
			); 
	}

	ShadowRenderer( 
		_In_ ID3D11Device1 *d3dDevice, 
		_In_ ID3D11DeviceContext1 *d3dContext
		); 

	void OnFrameMove( 
		_In_ ShadowRenderer::TimerView *timer = nullptr
		);

	REIGN_FORCEINLINING void SetColorTextureView(
		_In_ ID3D11ShaderResourceView *texView
		)
	{
		m_shadowPixelShaderTextureViewArray[0] = texView; // t0
	}

	REIGN_FORCEINLINING void SetDepthTextureView(
		_In_ ID3D11ShaderResourceView *texView, _In_ uint32 index
		)
	{
		m_shadowPixelShaderTextureViewArray[index + 1] = texView; // t1
	}

	void CreateResources(
		);

	void UnbindRenderTargetFromInput();

};