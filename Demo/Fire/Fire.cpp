#include "Fire.h"
#include "Engine.h"

void Demo::FeuerFrei::CreateDeviceDependentResources()
{
	using namespace Nena::Graphics;
	OutputDebugStringA("Demo::FeuerFrei::CreateDeviceDependentResources()\n");

	HRESULT result = S_OK;

	Graphics *graphics = Engine::GetForCurrentThread();
	Nena::App::Vars *vars = Nena::App::Vars::GetForCurrentThread();
	Resources::IDirect3DDevice *device = graphics->D3D.Device.Get();
	Resources::IDirect3DContext *context = graphics->D3D.Context.Get();
	Resources::IWICImagingFactory *wic = graphics->D2D.WICFactory.Get();
	_ASSERT_EXPR(device && wic, L"Engine is not yet ready");

	result = Resources::Assist::LoadTexture(
		device, wic, 
		vars->GetRecord<Nena::App::Vars::String16>("Fire/Textures/Fire/DDS"),
		FirePS.Rsrc.ReleaseAndGetAddressOf(),
		FirePS.Read.ReleaseAndGetAddressOf()
		); 
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot load texture");

	result = Resources::Assist::LoadTexture(
		device, wic,
		vars->GetRecord<Nena::App::Vars::String16>("Fire/Textures/Noise/DDS"),
		NoisePS.Rsrc.ReleaseAndGetAddressOf(),
		NoisePS.Read.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot load texture");

	result = Resources::Assist::LoadTexture(
		device, wic,
		vars->GetRecord<Nena::App::Vars::String16>("Fire/Textures/Alpha/DDS"),
		AlphaPS.Rsrc.ReleaseAndGetAddressOf(),
		AlphaPS.Read.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot load texture");

	Resources::VSInputElementDescription layoutDesc[2] =
	{ 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	result = Resources::Assist::LoadShader(
		device,
		vars->GetRecord<Nena::App::Vars::String16>("Fire/Shaders/Fire/VS/CSO"),
		layoutDesc,
		ARRAYSIZE(layoutDesc), 
		PipeVS.ReleaseAndGetAddressOf(),
		InputIA.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot load shader");

	result = Resources::Assist::LoadShader(
		device,
		vars->GetRecord<Nena::App::Vars::String16>("Fire/Shaders/Fire/PS/CSO"),
		PipePS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot load shader");

	Resources::SamplerStateDescription samplerDesc;
	Resources::Assist::Defaults(samplerDesc);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	result = device->CreateSamplerState(
		&samplerDesc, WrapPS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create sampler");

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = device->CreateSamplerState(
		&samplerDesc, ClampPS.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create sampler");


	Resources::RasterizerDescription rasterDesc;
	ZeroMemory(&rasterDesc, sizeof rasterDesc);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	//rasterDesc.FrontCounterClockwise = true;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	Resources::BlendStateDescription blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof blendStateDescription);

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	device->CreateRasterizerState1(&rasterDesc, RS.ReleaseAndGetAddressOf());
	device->CreateBlendState1(&blendStateDescription, BlendOM.ReleaseAndGetAddressOf());

	NoiseData.SetFrameTime(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/FrameTime"));
	NoiseData.SetScaleX(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScaleX"));
	NoiseData.SetScaleY(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScaleY"));
	NoiseData.SetScaleZ(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScaleZ"));
	NoiseData.SetScrollX(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScrollX"));
	NoiseData.SetScrollY(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScrollY"));
	NoiseData.SetScrollZ(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/VS/Noise/ScrollZ"));

	DistortionData.SetDistortion1X(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/1X"));
	DistortionData.SetDistortion1Y(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/1Y"));
	DistortionData.SetDistortion2X(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/2X"));
	DistortionData.SetDistortion2Y(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/2Y"));
	DistortionData.SetDistortion3X(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/3X"));
	DistortionData.SetDistortion3Y(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/3Y"));
	DistortionData.SetDistortionBias(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/Bias"));
	DistortionData.SetDistortionBias(vars->GetRecord<FLOAT>("Fire/Shaders/Fire/PS/Distortion/Scale"));

	result = MatricesVS.Create(device);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create buffer");
	result = NoiseVS.Create(device);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create buffer");
	result = DistortionPS.Create(device);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create buffer");

	Cam.AspectRatio = graphics->D3D.RenderTargetSize.Width /
		Demo::Engine::GetForCurrentThread()->D3D.RenderTargetSize.Height;
	Cam.Displacement.z = -2.5f;
	Cam.UpdateView({ 0, 0 }, 0);
	Cam.UpdateProjection();

	SpatialData.Packs[0] = Nena::Matrix::CreateTranslation(0, 0, 0);
	SpatialData.Packs[1] = Cam.View;
	SpatialData.Packs[2] = Cam.Projection;

	result = MatricesVS.SetData(context, SpatialData);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot update buffer");
	result = NoiseVS.SetData(context, NoiseData);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot update buffer");
	result = DistortionPS.SetData(context, DistortionData);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot update buffer");

	Vertex squareVertices[4]
	{
		{ { -1, +1, 0, +1 }, { 0, 0 } },
		{ { +1, +1, 0, +1 }, { 1, 0 } },
		{ { +1, -1, 0, +1 }, { 1, 1 } },
		{ { -1, -1, 0, +1 }, { 0, 1 } },
	};

	UINT16 squareIndices[6] = 
	{ 
		0, 1, 2,
		0, 2, 3,
	};

	result = Resources::Assist::CreateVertexBuffer(
		device, ARRAYSIZE(squareVertices), squareVertices, 
		VerticesIA.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create buffer");

	result = Resources::Assist::CreateIndexBuffer(
		device, ARRAYSIZE(squareIndices), squareIndices,
		IndicesIA.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot create buffer");



}

void Demo::FeuerFrei::CreateDeviceIndependentResources()
{
	OutputDebugStringA("Demo::FeuerFrei::CreateDeviceIndependentResources()\n");
}

void Demo::FeuerFrei::CreateWindowSizeDependentResources()
{
	OutputDebugStringA("Demo::FeuerFrei::CreateWindowSizeDependentResources()\n");
}

void Demo::FeuerFrei::CreateResources()
{
	OutputDebugStringA("Demo::FeuerFrei::CreateResources()\n");

	CreateDeviceIndependentResources();
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

Demo::FeuerFrei::FeuerFrei()
{
	OutputDebugStringA("Demo::FeuerFrei::FeuerFrei()\n");
	CreateResources();

	// TODO: Figure out why does wic factory corrupts?
	Engine::GetForCurrentThread()->D2D.WICFactory.Reset();
}

void Demo::FeuerFrei::Update()
{

	using namespace ::Nena::Graphics;
	using namespace ::Nena::Graphics::Resources;
	Graphics *graphics = Engine::GetForCurrentThread();
	Resources::IDirect3DDevice *device = graphics->D3D.Device.Get();
	Resources::IDirect3DContext *context = graphics->D3D.Context.Get();

	FLOAT totalTime = (FLOAT) Nena::App::GetForCurrentThread()->GetTimer()->GetTotalSeconds();
	NoiseData.SetFrameTime(totalTime);

	::HRESULT result = NoiseVS.SetData(context, NoiseData);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot update buffer");
	result = DistortionPS.SetData(context, DistortionData);
	_ASSERT_EXPR(SUCCEEDED(result), L"Cannot update buffer");
}

void Demo::FeuerFrei::Render()
{
	using namespace ::Nena::Graphics;
	using namespace ::Nena::Graphics::Resources;
	static const UINT32 strides[] = { sizeof Vertex };
	static const UINT32 offsets[] = { 0 };
	static const FLOAT factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const auto mask = 0xffffffff;

	::HRESULT result;

	Graphics *graphics = Engine::GetForCurrentThread();
	Resources::IDirect3DDevice *device = graphics->D3D.Device.Get();
	Resources::IDirect3DContext *context = graphics->D3D.Context.Get();

	graphics->D3D.Clear(context);
	graphics->D3D.BindToPipeline(context);

	context->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, VerticesIA.GetAddressOf(), strides, offsets);
	context->IASetIndexBuffer(IndicesIA.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetInputLayout(InputIA.Get());

	context->VSSetShader(PipeVS.Get(), nullptr, 0);
	context->PSSetShader(PipePS.Get(), nullptr, 0);

	context->VSSetConstantBuffers(0, 1, MatricesVS.GetBufferAddress());
	context->VSSetConstantBuffers(1, 1, NoiseVS.GetBufferAddress());

	context->PSSetConstantBuffers(0, 1, DistortionPS.GetBufferAddress());
	context->PSSetShaderResources(0, 1, FirePS.Read.GetAddressOf());
	context->PSSetShaderResources(1, 1, NoisePS.Read.GetAddressOf());
	context->PSSetShaderResources(2, 1, AlphaPS.Read.GetAddressOf());
	context->PSSetSamplers(0, 1, WrapPS.GetAddressOf());
	context->PSSetSamplers(1, 1, ClampPS.GetAddressOf());

	context->RSSetState(RS.Get());
	context->OMSetBlendState(BlendOM.Get(), factor, mask);

	context->DrawIndexed(6, 0, 0);

	context->PSSetShader(nullptr, nullptr, 0);
	context->VSSetShader(nullptr, nullptr, 0);

	result = graphics->D3D.Present();
	graphics->D3D.UnbindFromPipeline(context);
}



