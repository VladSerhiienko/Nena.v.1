#pragma once

#include "pch.h"

ref class RenderTarget
{
private:

	Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice; // d3d
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext; // d3d

	/// Resources

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTarget; // rt
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencil; // rt

	/// Resource views

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView; // rtv
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView; // rtv
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView; // srv
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_unorderedAccessView; // uav

	D3D11_VIEWPORT m_viewport;
	Reign::Vector4 m_clear;
	Reign::Matrix m_ortho;
	Reign::Matrix m_proj;

internal:

	inline ID3D11UnorderedAccessView **GetWriteViewAddress() { return m_unorderedAccessView.GetAddressOf(); };
	inline ID3D11UnorderedAccessView *GetWriteView() { return m_unorderedAccessView.Get(); };
	inline ID3D11ShaderResourceView *GetReadView() { return m_shaderResourceView.Get(); };
	inline D3D11_VIEWPORT *EditViewport() { return &m_viewport; }
	inline Reign::Vector4 *EditClear() { return &m_clear; };
	inline Reign::Matrix *EditOrtho() { return &m_ortho; };
	inline Reign::Matrix *EditProj() { return &m_proj; };

	inline ID3D11ShaderResourceView const *GetView() const { return m_shaderResourceView.Get(); };
	inline D3D11_VIEWPORT const *GetViewport() const { return &m_viewport; }
	inline Reign::Vector4 const *GetClear() const {  return &m_clear; };
	inline Reign::Matrix const *GetOrtho() const { return &m_ortho; };
	inline Reign::Matrix const *GetProj() const { return &m_proj; };
	inline Reign::Vector4 GetClearCpy() const { return m_clear; };
	inline Reign::Matrix GetOrthoCpy() const { return m_ortho; };
	inline Reign::Matrix GetProjCpy() const { return m_proj; };

	inline void Unbind()
	{
		static ID3D11RenderTargetView *s_null[] = { nullptr };
		m_d3dContext->OMSetRenderTargets(1, s_null, nullptr);
		//m_d3dContext->RSSetViewports(1, &m_viewport);
	}

internal:

	RenderTarget( 
		_In_ ID3D11Device1 *d3dDevice, 
		_In_ ID3D11DeviceContext1 *d3dContext
		); 

	void CreateResources( 
		_In_ uint32 textureWidth, _In_ uint32 textureHeight, 
		_In_ float32 nearPlane = 0.01f, _In_ float32 farPlane = 100.0f
		);

	void Clear();
	void Bind();

};