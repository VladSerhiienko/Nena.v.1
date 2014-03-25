#include <Nena/WICLoader.h>

#include "ShapeRenderer.h"
#include "Camera.h"
#include "Light.h"

void Demo::ShapeRenderer::CreateWindowSizeDependentResources()
{
}

void Demo::ShapeRenderer::CreateDeviceIndependentResources()
{
}

void Demo::ShapeRenderer::CreateDeviceDependentResources()
{
	LoadShaders();
	LoadBuffers();
	LoadTextures();
	LoadSamplers();
}

void Demo::ShapeRenderer::LoadShaders()
{
	using namespace ::Nena::Graphics::Resources;

	VSInputElementDescription desc[3] = 
	{
		{ "VSVERTEX0INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VSVERTEX1INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VSVERTEX2INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Assist::LoadShader(
		__D3Dd(), L"Shaders/ShapeVertexShader.cso", desc, ARRAYSIZE(desc), 
		Vs.ReleaseAndGetAddressOf(), Vi.ReleaseAndGetAddressOf()
		);

	Assist::LoadShader(__D3Dd(), L"Shaders/ShapeGeometryShader.cso", Gs.ReleaseAndGetAddressOf());
	Assist::LoadShader(__D3Dd(), L"Shaders/ShapePixelShader.cso", Ps.ReleaseAndGetAddressOf());
}

void Demo::ShapeRenderer::LoadTextures()
{
	using namespace ::Nena::Platform;
	using namespace ::Nena::Graphics::Resources;
	typedef ScopedObject<IDirect3DResource> ScopedD3DResource;

	Assist::LoadTexture(
		__D3Dd(), D2DResources->WICFactory.Get(), 
		ShapeTexturePath.c_str(),
		ShapeTexture.Rsrc.ReleaseAndGetAddressOf(),
		ShapeTexture.Read.ReleaseAndGetAddressOf()
		);

#if 0

	ScopedD3DResource shapeTextureResource;
	Nena::Utility::CreateWICTextureFromFile(__D3Dd(), __D3Dc(), ShapeTexturePath.c_str(),
		shapeTextureResource.ReleaseAndGetAddressOf(),
		ShapeTexture.Read.ReleaseAndGetAddressOf()
		);

	shapeTextureResource.As(&ShapeTexture.Rsrc);
	//ShapeTexture.Rsrc.As(&shapeTextureResource);
	shapeTextureResource.Reset();

#endif

}

void Demo::ShapeRenderer::LoadSamplers()
{
	using namespace ::Nena::Graphics::Resources;

	SamplerStateDescription desc;
	Assist::Defaults(desc);
	__D3Dd()->CreateSamplerState(
		&desc, ShapeSampleType.ReleaseAndGetAddressOf()
		);
}

void Demo::ShapeRenderer::LoadBuffers()
{
	using namespace ::Nena::Graphics::Resources;

	Assist::CreateStructuredBuffer(__D3Dd(), 
		(UINT32) Pbdata->size(), Pbdata->data(),
		Pb.Rsrc.ReleaseAndGetAddressOf(),
		Pb.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	Assist::CreateStructuredBuffer(__D3Dd(),
		(UINT32) Nbdata->size(), Nbdata->data(),
		Nb.Rsrc.ReleaseAndGetAddressOf(),
		Nb.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	Assist::CreateStructuredBuffer(__D3Dd(),
		(UINT32) Tbdata->size(), Tbdata->data(),
		Tb.Rsrc.ReleaseAndGetAddressOf(),
		Tb.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	Assist::CreateVertexBuffer(__D3Dd(),
		(UINT32) Vbdata->size(), Vbdata->data(),
		Vb.ReleaseAndGetAddressOf()
		);

	Xb.Create(__D3Dd());
	Lb.Create(__D3Dd());
	UpdateBuffers();
}

void Demo::ShapeRenderer::UpdateBuffers()
{
	using namespace ::Nena::Graphics::Resources;
	static MappedSubresource subresource = { 0 };

	Xbdata.W = (*ShapeW);
	Xbdata.V = (Camera->View);
	Xbdata.P = (Camera->Projection);
	Lbdata.AC = (Light->AmbientColor);
	Lbdata.DC = (Light->DiffuseColor);
	Lbdata.D = (Light->DiffuseDirection);
	//Lbdata.D = ::Nena::XMVector4Transform(Lbdata.D, Xbdata.V);

	Xb.SetData(__D3Dc(), Xbdata);
	Lb.SetData(__D3Dc(), Lbdata);
}

void Demo::ShapeRenderer::Update()
{
	// ! update buffers
	UpdateBuffers();
}

void Demo::ShapeRenderer::Render()
{
	using namespace ::Nena::Graphics::Resources;
	static UINT32 strides[] = { sizeof Shape::FACE };
	static UINT32 offsets[] = { 0 };

	__D3Dc()->IASetPrimitiveTopology(PrimitiveTopology::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	__D3Dc()->IASetVertexBuffers(0, 1, Vb.GetAddressOf(), strides, offsets);
	__D3Dc()->IASetInputLayout(Vi.Get());

	__D3Dc()->VSSetShader(Vs.Get(), nullptr, 0);
	__D3Dc()->GSSetShader(Gs.Get(), nullptr, 0);
	__D3Dc()->PSSetShader(Ps.Get(), nullptr, 0);

	__D3Dc()->GSSetConstantBuffers(0, 1, Xb.GetBufferAddress());
	__D3Dc()->GSSetShaderResources(0, 1, Pb.Read.GetAddressOf());
	__D3Dc()->GSSetShaderResources(1, 1, Nb.Read.GetAddressOf());
	__D3Dc()->GSSetShaderResources(2, 1, Tb.Read.GetAddressOf());
	__D3Dc()->PSSetConstantBuffers(0, 1, Lb.GetBufferAddress());
	__D3Dc()->PSSetShaderResources(0, 1, ShapeTexture.Read.GetAddressOf());
	__D3Dc()->PSSetSamplers(0, 1, ShapeSampleType.GetAddressOf());

	__D3Dc()->Draw((::UINT32) Vbdata->size(), 0);

	__D3Dc()->PSSetShader(nullptr, nullptr, 0);
	__D3Dc()->GSSetShader(nullptr, nullptr, 0);
	__D3Dc()->VSSetShader(nullptr, nullptr, 0);

}


