
#include "SoftShadowRenderer.h"
#include "ImageFilter.h"

void Demo::SoftShadowRenderer::HardValidation()
{

}

void Demo::SoftShadowRenderer::CreateDeviceResources(Demo::ShadowRenderer *shadow)
{
	IL = shadow->IL;
	VS = shadow->VS;
	Light = shadow->Light;
	Camera = shadow->Camera;
	Meshes = shadow->Meshes;
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

	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/SoftShadow/GS/CSO"),
		GS.GetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create gs");
	result = Assist::LoadShader(
		D3DDvc, vars->GetRecord<_Str>("Demo/ShadowMapping/Shader/SoftShadow/PS/CSO"),
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

void Demo::SoftShadowRenderer::Update()
{

}

void Demo::SoftShadowRenderer::Render(Demo::ShadowRenderer *shadow)
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;
	static ISRView *s_nulls[] = { nullptr, nullptr };

	D3D->Clear(D3DCtx);
	D3D->BindToPipeline(D3DCtx);

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
		D3DCtx->PSSetConstantBuffers(0, 1, Light->LightColorBuffer.GetBufferAddress());

		D3DCtx->GSSetShaderResources(0, 1, mesh->Points.Read.GetAddressOf());
		D3DCtx->GSSetShaderResources(1, 1, mesh->Normals.Read.GetAddressOf());
		D3DCtx->GSSetShaderResources(2, 1, mesh->Texcoords.Read.GetAddressOf());

		D3DCtx->PSSetShaderResources(0, 1, mesh->ColorTexture.Read.GetAddressOf());
		D3DCtx->PSSetShaderResources(1, 1, shadow->RTR.GetAddressOf());

		D3DCtx->Draw(mesh->VertexCount, 0);

	}

	D3D->Present();

	D3DCtx->PSSetShaderResources(0, 2, s_nulls);
	D3D->UnbindFromPipeline(D3DCtx);
}