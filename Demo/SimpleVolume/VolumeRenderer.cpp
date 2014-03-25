#include "Engine.h"
#include "VolumeRenderer.h"

float Demo::VolumeResources::CubeVolume2div3(UINT32 i, UINT32 j, UINT32 k, UINT32 w, UINT32 h, UINT32 d)
{
	/*i -= w / 2;
	j -= h / 2;
	k -= d / 2;*/

	if (i <= 2 * w / 3 &&
		j <= 2 * h / 3 &&
		k <= 2 * d / 3)
		return 1.0f;
	else return 0.0f;
}

float Demo::VolumeResources::SphereVolume2div3(UINT32 i, UINT32 j, UINT32 k, UINT32 w, UINT32 h, UINT32 d)
{
	i -= w / 2;
	j -= h / 2;
	k -= d / 2;
	if (sqrtf(i * i + j * j + k * k) <= min(min(w, h), d))
		return 1.0f;
	else return 0.0f;
}

Demo::VolumeResources::VolumeResources()
{
	Width = 100;
	Height = 100;
	Depth = 100;

	ObbWidth = 2;
	ObbHeight = 2;
	ObbDepth = 2;

	ObbTransform.World = Nena::Matrix::CreateScale(1);
	//ObbTransform.World = Nena::Matrix::CreateScale(ObbWidth, ObbHeight, ObbDepth);
}


void Demo::VolumeResources::CreateDeviceResources(VolumeResources::VolumeCallbackTy rule)
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	auto engine = Engine::GetForCurrentThread();
	D3D = &engine->D3D;
	auto dev = D3D->Device.Get();
	D3DCtx = D3D->Context.Get();

	HRESULT result = S_OK;

	result = ObbPsResource.Create(dev);
	result = CameraPsResource.Create(dev);
	result = SpatialVsResource.Create(dev);

	if (!rule) rule = &SphereVolume2div3;
	//if (!rule) rule = &CubeVolume2div3;

	// ...

	std::vector<float> data;
	data.resize(Width * Height * Depth);

	float value;
	UINT32 i, j, k;
	for (i = 0; i < Width; i++)
	for (j = 0; j < Height; j++)
	for (k = 0; k < Depth; k++)
		value = rule(i, j, k, Width, Height, Depth),
		data[i * Height * Depth + j * Depth + k] = value;

	CTexture3Description desc(
		//DxgiFormat::DXGI_FORMAT_R16_FLOAT,
		DxgiFormat::DXGI_FORMAT_R32_FLOAT,
		(::UINT32) Width,
		(::UINT32) Height,
		(::UINT32) Depth, 1,
		D3D11_BIND_SHADER_RESOURCE | 
		D3D11_BIND_UNORDERED_ACCESS
		);

	SubresourceData subres;
	subres.pSysMem = data.data();
	subres.SysMemPitch = Width * sizeof (float);
	subres.SysMemSlicePitch = Width * Height * sizeof (float);

	result = dev->CreateTexture3D(&desc, &subres, ObbVsVolume.Rsrc.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create tex");
	result = dev->CreateShaderResourceView(ObbVsVolume.Rsrc.Get(), nullptr, ObbVsVolume.Read.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create read");
	result = dev->CreateUnorderedAccessView(ObbVsVolume.Rsrc.Get(), nullptr, ObbVsVolume.Write.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create write");

	/*Nena::Vector4 vertices[8] = 
	{
		{ -1, -1, -1, +1 },
		{ -1, +1, -1, +1 },
		{ +1, +1, -1, +1 },
		{ +1, -1, -1, +1 },
		{ -1, -1, +1, +1 },
		{ -1, +1, +1, +1 },
		{ +1, +1, +1, +1 },
		{ +1, -1, +1, +1 },
	};*/



	Nena::Vector4 vertices[] = {

		// Front Face (1-2-3-4)
		{ -1.0f, 1.0f, -1.0f, 1 },
		{ 1.0f, 1.0f, -1.0f, 1 },
		{ -1.0f, -1.0f, -1.0f, 1 },
		{ 1.0f, -1.0f, -1.0f, 1 },

		// Right Face (2-6-4-8)
		{ 1.0f, 1.0f, -1.0f, 1 },
		{ 1.0f, 1.0f, 1.0f, 1 },
		{ 1.0f, -1.0f, -1.0f, 1 },
		{ 1.0f, -1.0f, 1.0f, 1 },

		// Top Face (5-6-1-2)
		{ -1.0f, 1.0f, 1.0f, 1 },
		{ 1.0f, 1.0f, 1.0f, 1 },
		{ -1.0f, 1.0f, -1.0f, 1 },
		{ 1.0f, 1.0f, -1.0f, 1 },

		// Back Face (6-5-8-7)
		{ 1.0f, 1.0f, 1.0f, 1 },
		{ -1.0f, 1.0f, 1.0f, 1 },
		{ 1.0f, -1.0f, 1.0f, 1 },
		{ -1.0f, -1.0f, 1.0f, 1 },

		// Left Face (5-1-7-3)
		{ -1.0f, 1.0f, 1.0f, 1 },
		{ -1.0f, 1.0f, -1.0f, 1 },
		{ -1.0f, -1.0f, 1.0f, 1 },
		{ -1.0f, -1.0f, -1.0f, 1 },

		// Bottom Face (3-4-7-8)
		{ -1.0f, -1.0f, -1.0f, 1 },
		{ 1.0f, -1.0f, -1.0f, 1 },
		{ -1.0f, -1.0f, 1.0f, 1 },
		{ 1.0f, -1.0f, 1.0f, 1 }
	};

	/*UINT16 indices[36]
	{
		0, 1, 2,
		0, 2, 3,
		7, 6, 2,
		7, 2, 3,
		4, 5, 6,
		4, 6, 7,
		0, 1, 5,
		0, 5, 4,
		5, 1, 2,
		5, 2, 6,
		0, 4, 7,
		0, 7, 3,
	};*/

	result = Assist::CreateVertexBuffer(dev, ARRAYSIZE(vertices), vertices, ObbVsVertices.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create vb");
	//result = Assist::CreateIndexBuffer(dev, ARRAYSIZE(indices), indices, ObbVsIndices.ReleaseAndGetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create ib");

}

void Demo::VolumeResources::Update()
{

	HRESULT result = S_OK;

	ObbPsData.ObbWorld = ObbTransform.World;
	ObbPsData.ObbWorldInverse = ObbPsData.ObbWorld.Invert();

	ObbPsData.AabbMin.x = -1; // (FLOAT) ObbWidth / 2;
	ObbPsData.AabbMin.y = -1; // (FLOAT) ObbHeight / 2;
	ObbPsData.AabbMin.z = -1; // (FLOAT) ObbDepth / 2;
	ObbPsData.AabbMax.x = 1;  // (FLOAT) ObbWidth / 2;
	ObbPsData.AabbMax.y = 1;  // (FLOAT) ObbHeight / 2;
	ObbPsData.AabbMax.z = 1;  // (FLOAT) ObbDepth / 2;

	/*ObbPsData.AabbMin.x = -(FLOAT) ObbWidth / 2;
	ObbPsData.AabbMin.y = -(FLOAT) ObbHeight / 2;
	ObbPsData.AabbMin.z = -(FLOAT) ObbDepth / 2;
	ObbPsData.AabbMax.x = (FLOAT) ObbWidth / 2;
	ObbPsData.AabbMax.y = (FLOAT) ObbHeight / 2;
	ObbPsData.AabbMax.z = (FLOAT) ObbDepth / 2;*/

	CameraPsData.CameraProjection = Viewer.Projection;
	CameraPsData.CameraViewInverse = Viewer.View.Invert();
	CameraPsData.ScreenDims.x = D3D->RenderTargetSize.Width;
	CameraPsData.ScreenDims.y = D3D->RenderTargetSize.Height;

	SpatialVsData.World = ObbTransform.World;
	SpatialVsData.CameraView = Viewer.View;
	SpatialVsData.CameraProj = Viewer.Projection;

	result = ObbPsResource.SetData(D3DCtx, ObbPsData);
	result = CameraPsResource.SetData(D3DCtx, CameraPsData);
	result = SpatialVsResource.SetData(D3DCtx, SpatialVsData);
}

void Demo::VolumeRenderer::CreateDeviceResources(VolumeResources::VolumeCallbackTy rule)
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	auto engine = Engine::GetForCurrentThread();

	D3D = &engine->D3D;
	D3DCtx = D3D->Context.Get();

	HRESULT result = S_OK;

	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	VSInputElementDescription inputDesc[1] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	result = Assist::LoadShader(
		D3D->Device.Get(),
		vars->GetRecord<std::wstring>("Demo/SimpleVolume/Shader/VS/CSO"),
		inputDesc, 1, 
		Vs.ReleaseAndGetAddressOf(), 
		Il.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to find/load/create vs");

	result = Assist::LoadShader(
		D3D->Device.Get(),
		vars->GetRecord<std::wstring>("Demo/SimpleVolume/Shader/PS/CSO"),
		Ps.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to find/load/create ps");

	CSampleStateDescription linearSamplerDesc(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		D3D11_TEXTURE_ADDRESS_WRAP,
		0, 0, D3D11_COMPARISON_NEVER,
		nullptr, 0, D3D11_FLOAT32_MAX
		);
	result = D3D->Device->CreateSamplerState(
		&linearSamplerDesc, 
		Ss.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create sampler state");

	Resource.CreateDeviceResources(rule);

}

void Demo::VolumeRenderer::Update()
{
	Resource.Update();
}

void Demo::VolumeRenderer::Render()
{
	static UINT32 strides[] = { sizeof Nena::Vector4 };
	static UINT32 offsets[] = { 0 };

	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;
	static ISRView *s_nulls[] = { nullptr, nullptr };

	D3D->Clear(D3DCtx);
	D3D->BindToPipeline(D3DCtx);

	D3DCtx->VSSetShader(Vs.Get(), nullptr, 0);
	D3DCtx->PSSetShader(Ps.Get(), nullptr, 0);

	D3DCtx->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//D3DCtx->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3DCtx->IASetInputLayout(Il.Get());

	D3DCtx->IASetVertexBuffers(0, 1, Resource.ObbVsVertices.GetAddressOf(), strides, offsets);
	//D3DCtx->IASetIndexBuffer(Resource.ObbVsIndices.Get(), DXGI_FORMAT_R16_UINT, 0);

	D3DCtx->VSSetConstantBuffers(0, 1, Resource.SpatialVsResource.GetBufferAddress());

	D3DCtx->PSSetConstantBuffers(0, 1, Resource.CameraPsResource.GetBufferAddress());
	D3DCtx->PSSetConstantBuffers(1, 1, Resource.ObbPsResource.GetBufferAddress());
	D3DCtx->PSSetShaderResources(0, 1, Resource.ObbVsVolume.Read.GetAddressOf());
	D3DCtx->PSSetSamplers(0, 1, Ss.GetAddressOf());

	D3DCtx->Draw(24, 0);
	//D3DCtx->DrawIndexed(36, 0, 0);

	D3D->Present();

	D3DCtx->PSSetShaderResources(0, 2, s_nulls);
	D3D->UnbindFromPipeline(D3DCtx);


}

