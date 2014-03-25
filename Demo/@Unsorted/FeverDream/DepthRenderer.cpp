#include "pch.h"
#include "DepthRenderer.h"
#include "BasicLoader.h"

DepthRenderer::DepthRenderer( _In_ ID3D11Device1 *d3dDevice, _In_ ID3D11DeviceContext1 *d3dContext ) : 
	m_d3dDevice(d3dDevice), m_d3dContext(d3dContext)
{
}

void DepthRenderer::CreateResources()
{
	BasicLoader^ loader = ref new BasicLoader(m_d3dDevice.Get());
	{ // Load vertex shader
		uint32 numElements = 1;
		D3D11_INPUT_ELEMENT_DESC inputElement[1];
		inputElement[0].SemanticName = "POSITION";
		inputElement[0].SemanticIndex = 0;
		inputElement[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement[0].InputSlot = 0;
		inputElement[0].AlignedByteOffset = 0;
		inputElement[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputElement[0].InstanceDataStepRate = 0;

		loader->LoadShader(
			"DepthVertexShader.cso", inputElement, numElements, 
			m_depthVertexShader.GetAddressOf(),
			m_depthInputLayout.GetAddressOf()
			);
	}
	{ // Load pixel shader
		loader->LoadShader(
			"DepthPixelShader.cso",
			m_depthPixelShader.GetAddressOf()
			);
	}
	{ // Load spatial const buffer
		BasicLoader::CreateConstantBuffer<Spatial>(
			m_d3dDevice.Get(), m_spatial.GetAddressOf(), 
			&m_spatialData
			);
	}
}

void DepthRenderer::OnFrameMove( _In_ DepthRenderer::TimerView *timer )
{
	(void)timer;

	m_d3dContext->IASetInputLayout(m_depthInputLayout.Get());
	m_d3dContext->VSSetShader(m_depthVertexShader.Get(), nullptr, 0u);
	m_d3dContext->VSSetConstantBuffers(0, 1, m_spatial.GetAddressOf());
	m_d3dContext->PSSetShader(m_depthPixelShader.Get(), nullptr, 0u);
}

//void DepthRenderer::OnFrame()
//{
//	m_d3dContext->DrawIndexed(m_numIndices, 0, 0);
//}