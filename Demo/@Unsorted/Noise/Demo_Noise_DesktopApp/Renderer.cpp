#include "Precompiled.h"
#include "Renderer.h"

#include "PerlinNoise.h"
#include "Noise.h"

typedef char8 DefaultString[MAX_PATH];

Sandbox::Renderer::Renderer()
{
	PrintToDebugWindowOnly8("Sandbox. Creating renderer.");
	InitializeCriticalSection(&m_lock);

	/*{
		PerlinNoise2 perlinNoise;
		perlinNoise.Init();
		
		DefaultString msg;
		Reign::Vector2 sample;
		PrintToDebugWindowOnly8("original perlin noise:");
		for (decltype(perlinNoise.Base()) i = 0; i < perlinNoise.Base() / 2.44f; i++, sample.x = i)
		{
			for (decltype(perlinNoise.Base()) j = 0; j < perlinNoise.Base() / 2.44f; j++, sample.y = j)
			{
				float32 smoothij = perlinNoise.SmoothNoise(sample);
				float32 perlin2ij = perlinNoise.PerlinNoise(sample, 2);
				float32 perlin4ij = perlinNoise.PerlinNoise(sample, 4);
				float32 perlin8ij = perlinNoise.PerlinNoise(sample, 8);

				sprintf_s(msg, "[%4d-%-4d] [ %-2.5f | %-2.5f | %-2.5f | %-2.5f ] ",
					i, j, smoothij, perlin2ij, perlin4ij, perlin8ij
					); PrintToDebugWindowOnly8(msg);
			}
		}
	}*/
	{
		int32 width = 128;
		int32 height = 128;


		NoiseUtil::NoiseArray2 whiteNoise(width, height);
		NoiseUtil::NoiseArray2 smoothNoise(width, height);
		NoiseUtil::NoiseArray2 perlinNoise(width, height);

		NoiseUtil::RandomSeed(0x001);
		NoiseUtil::GenerateWhiteNoise(&whiteNoise);
		NoiseUtil::GenerateSmoothNoise(&smoothNoise, &whiteNoise, 4);
		NoiseUtil::GeneratePerlinNoise(&perlinNoise, &whiteNoise, 4);

		DefaultString msg;
		PrintToDebugWindowOnly8("header:");
		sprintf_s(msg, "[   i-j   ] [ white   | smooth  | perlin ] "
			); PrintToDebugWindowOnly8(msg);

		PrintToDebugWindowOnly8("noise:");
		for (decltype(width) i = 0; i < width; i++)
		{
			for (decltype(height) j = 0; j < height; j++)
			{
				auto whiteij = whiteNoise[i][j];
				auto smoothij = smoothNoise[i][j];
				auto perlinij = perlinNoise[i][j];

				sprintf_s(msg, "[%4d-%-4d] [ %.5f | %.5f | %.5f] ",
					i, j, whiteij, smoothij, perlinij
					); PrintToDebugWindowOnly8(msg);
			}
		}
	}
}

Sandbox::Renderer::~Renderer()
{
	PrintToDebugWindowOnly8("Sandbox. Deleting renderer.");

	DeleteCriticalSection(&m_lock);
}

void Sandbox::Renderer::CreateDeviceResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating device resources.");

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

		Sehnsucht::ThrowIfFailed(m_d3dDevice->CreateRasterizerState1(
			&rasterDesc, &m_rasterizerCcw
			));
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
}

void Sandbox::Renderer::CreateDeviceIndependentResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating device independent resources.");
}

void Sandbox::Renderer::CreateWindowSizeDependentResources()
{
	PrintToDebugWindowOnly8("Sandbox. Creating window size dependent resources.");
}

void Sandbox::Renderer::Initialize()
{
	PrintToDebugWindowOnly8("Sandbox. Initializing renderer.");
	EnterCriticalSection(&m_lock);

	DirectXBaseDesktop::Initialize();

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	LeaveCriticalSection(&m_lock);
}

void Sandbox::Renderer::UpdateForWindowSizeChange()
{
	PrintToDebugWindowOnly8("Sandbox. Updating renderer on window size changed.");
	EnterCriticalSection(&m_lock);

	DirectXBaseDesktop::UpdateForWindowSizeChange();

	m_windowSizeChangedArgs.Dim.y = m_d3dRenderTargetSize.height;
	m_windowSizeChangedArgs.Dim.x = m_d3dRenderTargetSize.width;
	m_windowSizeChangedArgs.Dpi.y = m_dpi;
	m_windowSizeChangedArgs.Dpi.x = m_dpi;

	LeaveCriticalSection(&m_lock);
}