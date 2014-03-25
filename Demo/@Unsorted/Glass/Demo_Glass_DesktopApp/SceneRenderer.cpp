#include "Precompiled.h"
#include "SceneRenderer.h"



Sandbox::SceneRenderer::SceneRenderer(_No_args_)
{
}

Sandbox::SceneRenderer::~SceneRenderer(_No_args_)
{
}

void Sandbox::SceneRenderer::Initialize(_No_args_)
{
	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();
}

void Sandbox::SceneRenderer::CreateWindowSizeDependentResources(_No_args_)
{

}

void Sandbox::SceneRenderer::CreateDeviceIndependentResources(_No_args_)
{

}

void Sandbox::SceneRenderer::CreateDeviceResources(_No_args_)
{
	{ // VS PS
		Sehnsucht::IO::BasicLoader loader(m_d3dDevice, m_wicFactory);
		auto loaderPtr = &loader;
		{ // VS CB
			loaderPtr->CreateConstantBuffer(
				m_sceneSpatial.GetAddressOf(), 
				&m_sceneSpatialData
				);
		}
		{ // VS
			D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
			ZeroValueArray(polygonLayout, ARRAYSIZE(polygonLayout));
			polygonLayout[0].SemanticName = "POSITION";
			polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[0].AlignedByteOffset = 0;
			polygonLayout[1].SemanticName = "NORMAL";
			polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[1].AlignedByteOffset = 16;
			polygonLayout[2].SemanticName = "TEXCOORD";
			polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
			polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			polygonLayout[2].AlignedByteOffset = 28;

			loaderPtr->LoadShader( 
				L"SceneVertexShader.cso", 
				polygonLayout, ARRAYSIZE(polygonLayout), 
				m_sceneVertexShader.GetAddressOf(), 
				m_sceneLayout.GetAddressOf()
				);
		}
		{ // PS
			loaderPtr->LoadShader( 
				L"ScenePixelShader.cso", 
				m_scenePixelShader.GetAddressOf()
				);
		}
		{ // Textures
			loaderPtr->LoadTexture(
				L"Assets/seafloor.dds",
				m_cubeColorTexture.GetAddressOf(), 
				m_cubeColorTextureView.GetAddressOf()
				);
		}
	}
	{ // IA
		Converter converter;
		Sehnsucht::BasicShapes::CreateCube(&converter, m_cubeVerticesData, m_cubeIndicesData);
		Sehnsucht::BasicShapes::CreateVertexBuffer(m_d3dDevice, m_cubeVerticesData.size(), m_cubeVerticesData.data(), m_cubeVertices.GetAddressOf());
		Sehnsucht::BasicShapes::CreateIndexBuffer(m_d3dDevice, m_cubeIndicesData.size(), m_cubeIndicesData.data(), m_cubeIndices.GetAddressOf());
	}
	{ // PS

		D3D11_SAMPLER_DESC samplerDesc;
		ZeroValue(&samplerDesc);

		// Create a texture sampler state description.
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_d3dDevice->CreateSamplerState(
			&samplerDesc, 
			m_sceneSampler.GetAddressOf()
			);
	}
}

void Sandbox::SceneRenderer::UpdateForWindowSizeChange(_In_ Sandbox::EventArgsForWindowSizeChange args)
{
}