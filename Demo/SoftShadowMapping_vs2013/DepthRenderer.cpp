#include "DepthRenderer.h"

#include "Engine.h"
#include "ImageFilter.h"
#include <Nena\AppVars.h>

#include <Nena\Assist.h>
#include <Nena\PlatformAssist.h>


Demo::DepthRenderer::DepthRenderer()
{
	auto engine = Engine::GetForCurrentThread();
	auto d3d = &engine->D3D;
	auto d2d = &engine->D2D;

	D3DDvc = d3d->Device.Get();
	D3DCtx = d3d->Context.Get();
}

Demo::DepthRenderer::~DepthRenderer()
{
}

void Demo::DepthRenderer::CreateDeviceResources()
{
	CreateRenderTargetResources();
	CreateRendererResources();
}

void Demo::DepthRenderer::CreateRendererResources()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;
	typedef Nena::App::Vars::String16 _Str;

	auto engine = Engine::GetForCurrentThread();
	auto d3d = &engine->D3D;
	auto d2d = &engine->D2D;

	UINT32 stride;
	Demo::ShapeResources::PipelineInput input;
	Demo::ShapeResources::GetInput(input, stride);

	HRESULT result = S_OK;
	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/Depth/VS/CSO"),
		input, ARRAYSIZE(input),
		VS.ReleaseAndGetAddressOf(), 
		IL.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create vs");
	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/Depth/GS/CSO"),
		GS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create gs");
	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/Depth/PS/CSO"),
		PS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create ps");

}

void Demo::DepthRenderer::CreateRenderTargetResources()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	_ASSERT_EXPR(Meshes, L"Mesh is missing");
	_ASSERT_EXPR(Meshes->size(), L"Mesh is missing");
	_ASSERT_EXPR(Light, L"Light is missing");
	_ASSERT_EXPR(Camera, L"Camera is missing");

	auto d3d = &Engine::GetForCurrentThread()->D3D;
	D3DDvc = d3d->Device.Get();
	D3DCtx = d3d->Context.Get();

	UINT32 textureWidth = ImageFilter::SizeX; // (UINT32) d3d->RenderTargetSize.Width * 2;
	UINT32 textureHeight = ImageFilter::SizeY; // (UINT32) d3d->RenderTargetSize.Height * 2;

	HRESULT result = S_OK;
	Texture2Description textureDesc;
	ZeroValue(&textureDesc);

	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	textureDesc.BindFlags
		= D3D11_BIND_RENDER_TARGET
		| D3D11_BIND_SHADER_RESOURCE
		| D3D11_BIND_UNORDERED_ACCESS;

	result = D3DDvc->CreateTexture2D(
		&textureDesc, nullptr, 
		RT.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create rt");

	RTVDescription renderTargetViewDesc;
	ZeroValue(&renderTargetViewDesc);

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = D3DDvc->CreateRenderTargetView(
		RT.Get(), &renderTargetViewDesc, 
		RenderTargetView.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create rtv");

	SRVDescription shaderResourceViewDesc;
	ZeroValue(&shaderResourceViewDesc);

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = D3DDvc->CreateShaderResourceView(
		RT.Get(), &shaderResourceViewDesc, 
		RTR.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create rt srv");

	UAVDescription unorderedAccessViewDesc;
	ZeroValue(&unorderedAccessViewDesc);

	// Setup the description of the unoredered access view.
	unorderedAccessViewDesc.Format = textureDesc.Format;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

	result = D3DDvc->CreateUnorderedAccessView(
		RT.Get(), &unorderedAccessViewDesc, 
		RTW.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create rt uav");

	
	Texture2Description depthBufferDesc;
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

	result = D3DDvc->CreateTexture2D(
		&depthBufferDesc, nullptr, 
		DS.GetAddressOf()
		);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroValue(&depthStencilViewDesc);

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = D3DDvc->CreateDepthStencilView(
		DS.Get(), &depthStencilViewDesc, 
		DepthStencilView.GetAddressOf()
		);

	Viewport.Width = (FLOAT) textureWidth;
	Viewport.Height = (FLOAT) textureHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

}

void Demo::DepthRenderer::Update()
{
}

void Demo::DepthRenderer::Render()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	Clear(D3DCtx);
	BindToPipeline(D3DCtx);

	D3DCtx->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3DCtx->IASetInputLayout(IL.Get());

	D3DCtx->VSSetShader(VS.Get(), nullptr, 0);
	D3DCtx->GSSetShader(GS.Get(), nullptr, 0);
	D3DCtx->PSSetShader(PS.Get(), nullptr, 0);
	D3DCtx->GSSetConstantBuffers(0, 1, Camera->ViewerBuffer.GetBufferAddress());
	//D3DCtx->GSSetConstantBuffers(1, 1, Light->LightSpatialBuffer.GetBufferAddress());
	//D3DCtx->PSSetConstantBuffers(0, 1, Light->LightColorBuffer.GetBufferAddress());

	UINT32 strides[1] = { 0 };
	UINT32 offsets[1] = { 0 };

	auto meshi = Meshes->begin();
	Demo::ShapeResources *mesh = nullptr;
	for (; meshi != Meshes->end(); meshi++)
	{
		mesh = (*meshi);
		strides[0] = mesh->Stride;
		//offsets[0] = mesh->Offset;

		Light->Update(&mesh->Shape->TransformOS.World, mesh->Light);

		D3DCtx->IASetVertexBuffers(0, 1, mesh->Indices.GetAddressOf(), strides, offsets);
		D3DCtx->GSSetConstantBuffers(0, 1, Light->LightDepthBuffer.GetBufferAddress());
		D3DCtx->GSSetShaderResources(0, 1, mesh->Points.Read.GetAddressOf());

		D3DCtx->Draw(mesh->VertexCount, 0);
	}

	UnbindFromPipeline(D3DCtx);
}

void Demo::DepthRenderer::HardValidation()
{
	Nena::App::Vars::HResultSync result = S_OK;
	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Depth/VS/CSO"), 
		L"DepthVS record is missing");
	_ASSERT_EXPR(vars->HasRecord("Demo/ShadowMapping/Shader/Depth/PS/CSO"), 
		L"DepthPS record is missing");
}