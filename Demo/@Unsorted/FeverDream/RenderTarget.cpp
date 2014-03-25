#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(  _In_ ID3D11Device1 *d3dDevice,  _In_ ID3D11DeviceContext1 *d3dContext ) : 
	m_d3dDevice(d3dDevice), m_d3dContext(d3dContext)
{
}

void RenderTarget::CreateResources( 
	_In_ uint32 textureWidth, _In_ uint32 textureHeight, 
	_In_ float32 nearPlane, _In_ float32 farPlane
	)
{
	D3D11_TEXTURE2D_DESC textureDesc; 
	ZeroValue(&textureDesc);

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, m_renderTarget.GetAddressOf()));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc; 
	ZeroValue(&renderTargetViewDesc);
	
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(m_renderTarget.Get(), &renderTargetViewDesc, m_renderTargetView.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc; 
	ZeroValue(&shaderResourceViewDesc);

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	DX::ThrowIfFailed(m_d3dDevice->CreateShaderResourceView(m_renderTarget.Get(), &shaderResourceViewDesc, m_shaderResourceView.GetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	ZeroValue(&unorderedAccessViewDesc);

	// Setup the description of the unoredered access view.
	unorderedAccessViewDesc.Format = textureDesc.Format;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	DX::ThrowIfFailed(m_d3dDevice->CreateUnorderedAccessView(m_renderTarget.Get(), &unorderedAccessViewDesc, m_unorderedAccessView.GetAddressOf()));

	D3D11_TEXTURE2D_DESC depthBufferDesc; 
	ZeroValue(&depthBufferDesc);

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencil.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroValue(&depthStencilViewDesc);

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf()));

	m_viewport.Width = (float32)textureWidth;
	m_viewport.Height = (float32)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_proj = Reign::Matrix::CreatePerspectiveFieldOfView(
		XM_PIDIV4, (float32)textureWidth / (float32)textureHeight, 
		nearPlane, farPlane
		);

	m_ortho = Reign::Matrix::CreateOrthographic(
		(float32)textureWidth, (float32)textureHeight, 
		nearPlane, farPlane
		);
}

void RenderTarget::Clear()
{
	// Clear back buffer and bound depth/stencil buffer
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), reinterpret_cast<float32*>(&m_clear));
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0ui8);
}

void RenderTarget::Bind()
{
	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_d3dContext->RSSetViewports(1, &m_viewport);
}