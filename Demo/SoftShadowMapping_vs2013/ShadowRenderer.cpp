
#include "ShadowRenderer.h"
#include "Engine.h"
#include "ImageFilter.h"


Demo::ShadowRenderer::ShadowRenderer()
{
}

Demo::ShadowRenderer::~ShadowRenderer()
{
	delete Filter;
}

void Demo::ShadowRenderer::HardValidation()
{

}

void Demo::ShadowRenderer::CreateDeviceResources(Demo::DepthRenderer *depth)
{
	IL = depth->IL;
	VS = depth->VS;
	Light = depth->Light;
	Camera = depth->Camera;
	Meshes = depth->Meshes;
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	_ASSERT_EXPR(Meshes, L"Mesh is missing");
	_ASSERT_EXPR(Meshes->size(), L"Mesh is missing");
	_ASSERT_EXPR(Light, L"Light is missing");
	_ASSERT_EXPR(Camera, L"Camera is missing");
	typedef Nena::App::Vars::String16 _Str;

	HRESULT result = S_OK;
	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	D3D = &Engine::GetForCurrentThread()->D3D;
	D3DDvc = D3D->Device.Get();
	D3DCtx = D3D->Context.Get();

	UINT32 textureWidth = ImageFilter::SizeX; // (UINT32) d3d->RenderTargetSize.Width * 2;
	UINT32 textureHeight = ImageFilter::SizeY; // (UINT32) d3d->RenderTargetSize.Height * 2;

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

	Filter = ImageFilter::CreateFilterFromVars();
	_ASSERT_EXPR(Filter, L"Cannot find depth rt filter");

	Filter->CreateDeviceResources(RT.Get());

	Viewport.Width = (FLOAT) textureWidth;
	Viewport.Height = (FLOAT) textureHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/Shadow/GS/CSO"),
		GS.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create gs");
	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/Shadow/PS/CSO"),
		PS.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create ps");

	Resources::SamplerStateDescription samplerDesc;
	Resources::Assist::Defaults(samplerDesc);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = D3DDvc->CreateSamplerState(
		&samplerDesc, WrapPS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create sampler");

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = D3DDvc->CreateSamplerState(
		&samplerDesc, ClampPS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create sampler");

}

void Demo::ShadowRenderer::Update()
{

}

void Demo::ShadowRenderer::Render(Demo::DepthRenderer *depth)
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;
	static ISRView *s_nulls [] = { nullptr, nullptr };

	Clear(D3DCtx);
	BindToPipeline(D3DCtx);
	/*D3D->Clear(D3DCtx);
	D3D->BindToPipeline(D3DCtx);*/

	D3DCtx->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3DCtx->IASetInputLayout(IL.Get());

	D3DCtx->VSSetShader(VS.Get(), nullptr, 0);
	D3DCtx->GSSetShader(GS.Get(), nullptr, 0);
	D3DCtx->PSSetShader(PS.Get(), nullptr, 0);
	D3DCtx->PSSetSamplers(0, 1, ClampPS.GetAddressOf());
	D3DCtx->PSSetSamplers(1, 1, WrapPS.GetAddressOf());

	UINT32 strides[1] = { 0 };
	UINT32 offsets[1] = { 0 };

	auto meshi = Meshes->begin();
	Demo::ShapeResources *mesh = nullptr;
	for (; meshi != Meshes->end(); meshi++)
	{
		mesh = (*meshi);
		strides[0] = mesh->Stride;

		Camera->Update(&mesh->Shape->TransformOS.World, mesh->Viewer);

		D3DCtx->IASetVertexBuffers(0, 1, mesh->Indices.GetAddressOf(), strides, offsets);

		D3DCtx->GSSetConstantBuffers(0, 1, Camera->ViewerBuffer.GetBufferAddress());
		D3DCtx->GSSetConstantBuffers(1, 1, Light->LightSpatialBuffer.GetBufferAddress());
		//D3DCtx->PSSetConstantBuffers(0, 1, Light->LightColorBuffer.GetBufferAddress());

		D3DCtx->GSSetShaderResources(0, 1, mesh->Points.Read.GetAddressOf());
		D3DCtx->GSSetShaderResources(1, 1, mesh->Normals.Read.GetAddressOf());
		D3DCtx->GSSetShaderResources(2, 1, mesh->Texcoords.Read.GetAddressOf());

		//D3DCtx->PSSetShaderResources(0, 1, mesh->ColorTexture.Read.GetAddressOf());
		//D3DCtx->PSSetShaderResources(1, 1, depth->RTR.GetAddressOf());
		D3DCtx->PSSetShaderResources(0, 1, depth->RTR.GetAddressOf());

		D3DCtx->Draw(mesh->VertexCount, 0);

	}

	D3DCtx->PSSetShaderResources(0, 2, s_nulls);
	UnbindFromPipeline(D3DCtx);

	/*D3D->Present();
	D3DCtx->PSSetShaderResources(0, 2, s_nulls);
	D3D->UnbindFromPipeline(D3DCtx);*/

	Filter->Render(RTR.Get(), RTW.Get());

}