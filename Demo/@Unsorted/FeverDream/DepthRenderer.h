#pragma once

#include "pch.h"

ref class DepthRenderer
{
private:

	typedef Application::Utility::Timing::TimerView TimerView;

	struct Spatial
	{
		Reign::Matrix g_World;
		Reign::Matrix g_View;
		Reign::Matrix g_Proj;
	};

	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_depthVertexShader; // vs
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_depthInputLayout; // vs il
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_depthPixelShader; // ps
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spatial; // vs cb
	boolean m_updateSpatial;
	Spatial m_spatialData;
	uint32 m_numIndices;

internal:

	REIGN_FORCEINLINING void SetNumIndices( _In_ uint32 numIndices ) { m_numIndices = numIndices; }
	REIGN_FORCEINLINING Spatial *EditSpatial() { m_updateSpatial = true; return &m_spatialData; } 
	REIGN_FORCEINLINING Spatial const *GetSpatial() const { return &m_spatialData; } 

	DepthRenderer( _In_ ID3D11Device1 *d3dDevice, _In_ ID3D11DeviceContext1 *d3dContext ); 
	void CreateResources();

	void OnFrameMove( _In_ DepthRenderer::TimerView *timer = nullptr );

	REIGN_FORCEINLINING void OnFrame() 
	{ 
		/*if (m_updateSpatial)*/ 
		m_d3dContext->UpdateSubresource(
			m_spatial.Get(), 0, nullptr, 
			(&m_spatialData), 0, 0
			);

		m_d3dContext->DrawIndexed(m_numIndices, 0, 0); 
	}

};