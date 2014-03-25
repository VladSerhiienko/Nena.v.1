#include "Precompiled.h"
#include "Renderer.h"

Sandbox::Renderer::Renderer()
{
	PrintToDebugWindowOnly8("Sandbox. Creating renderer.");
	InitializeCriticalSection(&m_lock);

	m_sceneRenderer = alloc_new SceneRenderer;
	m_glassRenderer = alloc_new GlassRenderer;
}

Sandbox::Renderer::~Renderer()
{
	PrintToDebugWindowOnly8("Sandbox. Deleting renderer.");

	_Delete_pointer(m_sceneRenderer);
	_Delete_pointer(m_glassRenderer);

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

	m_glassRenderer->WICFactory(m_wicFactory.Get());
	m_sceneRenderer->WICFactory(m_wicFactory.Get());
	m_glassRenderer->Context3(m_d3dContext.Get());
	m_sceneRenderer->Context3(m_d3dContext.Get());
	m_glassRenderer->Device3(m_d3dDevice.Get());
	m_sceneRenderer->Device3(m_d3dDevice.Get());
	m_sceneRenderTarget.Context3(m_d3dContext.Get());
	m_sceneRenderTarget.Device3(m_d3dDevice.Get());

	m_sceneRenderTarget.Height(m_d3dRenderTargetSize.height);
	m_sceneRenderTarget.Width(m_d3dRenderTargetSize.width);
	m_sceneRenderTarget.Initialize();

	(*m_sceneRenderTarget.EditViewport()) = m_viewport;
	m_sceneRenderTarget.EditClear()->x = 0.4f;
	m_sceneRenderTarget.EditClear()->y = 0.4f;
	m_sceneRenderTarget.EditClear()->z = 0.4f;
	m_sceneRenderTarget.EditClear()->w = 1.0f;
	m_clear = m_sceneRenderTarget.GetClearCpy();

	m_camera.Aspect(m_d3dRenderTargetSize.width / m_d3dRenderTargetSize.height);
	m_camera.Fov(Reign::XM_PIDIV4);
	m_camera.Near(100.0f);
	m_camera.Far(0.01f);
	(*m_camera.EditFocus()) = Reign::Vector3(0.0f, 0.0f, 10.0f);
	(*m_camera.EditEye()) = Reign::Vector3(0.0f, 0.0f, -15.0f);

	m_camera.UpdateProj();
	m_camera.UpdateView();

	m_glassRenderer->SceneRenderTarget(&m_sceneRenderTarget);
	m_glassRenderer->Initialize();
	m_sceneRenderer->Initialize();

	m_sceneRenderer->EditSceneSpatial()->World = Matrix::CreateTranslation(0.0f, 0.0f, 20.0f);
	m_sceneRenderer->EditSceneSpatial()->View = m_camera.GetViewCpy();
	m_sceneRenderer->EditSceneSpatial()->Proj = m_camera.GetProjCpy();

	m_glassRenderer->EditGlassSpatial()->World = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	//  * Matrix::CreateRotationY(Reign::XM_PIDIV4);
	m_glassRenderer->EditGlassSpatial()->View = m_camera.GetViewCpy();
	m_glassRenderer->EditGlassSpatial()->Proj = m_camera.GetProjCpy();
	m_glassRenderer->EditGlassPerturb()->RefractionPerturb = 0.01;

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

	m_glassRenderer->UpdateForWindowSizeChange(
		m_windowSizeChangedArgs
		);
	m_sceneRenderer->UpdateForWindowSizeChange(
		m_windowSizeChangedArgs
		);

	LeaveCriticalSection(&m_lock);
}