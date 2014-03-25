#include "Precompiled.h"
#include "FireRenderer.h"

Sandbox::FireRenderer::FireRenderer() : m_busy(true)
{
}

Sandbox::FireRenderer::~FireRenderer()
{
	m_busy = false;
}

void Sandbox::FireRenderer::CreateDeviceIndependentResources()
{

	Sehnsucht::IO::BasicLoader loader(m_d3dDevice, m_wicFactory);
	auto loaderPtr = &loader;
	{ // Load vertex shader
		D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
		ZeroValueArray(polygonLayout, ARRAYSIZE(polygonLayout));
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[1].SemanticName = "TEXCOORD";
		polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].AlignedByteOffset = 16;

		loaderPtr->LoadShader(
			L"FireVertexShader.cso",
			polygonLayout, ARRAYSIZE(polygonLayout),
			m_fireVertexShader.GetAddressOf(),
			m_fireInputLayout.GetAddressOf()
			);
	}
	{ // Load pixel shader
		loaderPtr->LoadShader(
			L"FirePixelShader.cso",
			m_firePixelShader.GetAddressOf()
			);
	}
	{ // Create constant buffers
		loaderPtr->CreateConstantBuffer(m_fireDistortion.GetAddressOf(), &m_fireDistortionData);
		loaderPtr->CreateConstantBuffer(m_fireSpatial.GetAddressOf(), &m_fireSpatialData);
		loaderPtr->CreateConstantBuffer(m_fireNoise.GetAddressOf(), &m_fireNoiseData);
		m_fireVertexShaderBufferArray[0] = m_fireSpatial.Get();
		m_fireVertexShaderBufferArray[1] = m_fireNoise.Get();
	}
	{ // Create sampler states

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

		m_d3dDevice->CreateSamplerState(&samplerDesc, m_wrapSampler.GetAddressOf());

		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		m_d3dDevice->CreateSamplerState(&samplerDesc, m_clampSampler.GetAddressOf());

		m_firePixelShaderSamplerStateArray[0] = m_wrapSampler.Get();
		m_firePixelShaderSamplerStateArray[1] = m_clampSampler.Get();

	}
	{ // Load textures
		loaderPtr->LoadTexture(L"Assets/fire01.dds", m_fireColorTexture.GetAddressOf(), m_fireColorTextureView.GetAddressOf());
		loaderPtr->LoadTexture(L"Assets/noise01.dds", m_fireNoiseTexture.GetAddressOf(), m_fireNoiseTextureView.GetAddressOf());
		loaderPtr->LoadTexture(L"Assets/alpha01.dds", m_fireBodyTexture.GetAddressOf(), m_fireBodyTextureView.GetAddressOf());
		m_firePixelShaderTextureArray[0] = m_fireColorTextureView.Get();
		m_firePixelShaderTextureArray[1] = m_fireNoiseTextureView.Get();
		m_firePixelShaderTextureArray[2] = m_fireBodyTextureView.Get();
	}
	{ // Create alpha on blend state
		Rendering::BlendStateDescrition blendStateDescription;
		ZeroValue(&blendStateDescription);

		// Create an alpha enabled blend state description.
		blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

		m_d3dDevice->CreateBlendState1(&blendStateDescription, m_blendingEnabled.GetAddressOf()); // !supp win7
	}
	{ // Create rasterizer state
		Rendering::RasterizerStateDescrition rasterDesc;
		ZeroValue(&rasterDesc);

		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = true;
		//rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		m_d3dDevice->CreateRasterizerState1(&rasterDesc, &m_fireRasterizer);
	}
	{ // Create fire vertex and index buffers
		Converter converter;

		//Sehnsucht::BasicShapes::CreateCube(
		//	&converter, //Reign::Vector3(5.0f, 5.0f, 5.0f),
		//	m_vertices, m_indices
		//	);
		Sehnsucht::BasicShapes::CreateVerticalRect(
			&converter, Reign::Vector2(10.3f, 10.3f),
			m_vertices, m_indices
			);
		Sehnsucht::BasicShapes::CreateVertexBuffer(
			m_d3dDevice, _To_uint m_vertices.size(), m_vertices.data(),
			m_fireVertices.GetAddressOf()
			);
		Sehnsucht::BasicShapes::CreateIndexBuffer(
			m_d3dDevice, _To_uint m_indices.size(), m_indices.data(),
			m_fireIndices.GetAddressOf()
			);
	}

	loaderPtr = nullptr;
}

void Sandbox::FireRenderer::CreateWindowSizeDependentResources()
{
}

void Sandbox::FireRenderer::CreateDeviceResources()
{
}

void Sandbox::FireRenderer::UpdateForWindowSizeChange()
{
}

void Sandbox::FireRenderer::Initialize()
{
	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();
}
