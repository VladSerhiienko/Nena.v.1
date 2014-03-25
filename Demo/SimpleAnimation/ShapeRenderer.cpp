#include "ShapeRenderer.h"
#include "Camera.h"

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
	//UpdateBuffers();
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

	Assist::LoadShader(__D3Dd(), L"Shaders/ShapeVertexShader.cso", desc, ARRAYSIZE(desc), Vs.GetAddressOf(), Vi.GetAddressOf());
	Assist::LoadShader(__D3Dd(), L"Shaders/ShapeGeometryShader.cso", Gs.GetAddressOf());
	Assist::LoadShader(__D3Dd(), L"Shaders/ShapePixelShader.cso", Ps.GetAddressOf());
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

	Xbdata.W = (*ShapeW);
	Xbdata.V = (Camera->View);
	Xbdata.P = (Camera->Projection);
	Assist::CreateConstantBuffer(__D3Dd(), 
		&Xbdata, Xb.ReleaseAndGetAddressOf()
		);
}

void Demo::ShapeRenderer::UpdateBuffers()
{
	using namespace ::Nena::Graphics::Resources;
	static MappedSubresource subresource = { 0 };

	//::HRESULT result;

	Xbdata.W = (*ShapeW);
	Xbdata.V = (Camera->View);
	Xbdata.P = (Camera->Projection);

	/*Framework::Output(
		"view ----------------------------- \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n",
		Camera->View._11, Camera->View._12, Camera->View._13, Camera->View._14,
		Camera->View._21, Camera->View._22, Camera->View._23, Camera->View._24,
		Camera->View._31, Camera->View._32, Camera->View._33, Camera->View._34,
		Camera->View._41, Camera->View._42, Camera->View._43, Camera->View._44
		);

	Framework::Output(
		"proj ----------------------------- \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n"
		"%2.2f %2.2f %2.2f %2.2f \n",
		Camera->Projection._11, Camera->Projection._12, Camera->Projection._13, Camera->Projection._14,
		Camera->Projection._21, Camera->Projection._22, Camera->Projection._23, Camera->Projection._24,
		Camera->Projection._31, Camera->Projection._32, Camera->Projection._33, Camera->Projection._34,
		Camera->Projection._41, Camera->Projection._42, Camera->Projection._43, Camera->Projection._44
		);*/

	//MapWriteUnmap(__D3Dc(), Xb.Get(), subresource, &Xbdata, result);

	__D3Dc()->UpdateSubresource(Pb.Rsrc.Get(), 0, 0, Pbdata->data(), 0, 0);
	__D3Dc()->UpdateSubresource(Nb.Rsrc.Get(), 0, 0, Nbdata->data(), 0, 0);
	__D3Dc()->UpdateSubresource(Xb.Get(), 0, 0, &Xbdata, 0, 0);
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
	__D3Dc()->GSSetConstantBuffers(0, 1, Xb.GetAddressOf());
	__D3Dc()->GSSetShaderResources(0, 1, Pb.Read.GetAddressOf());
	__D3Dc()->GSSetShaderResources(1, 1, Nb.Read.GetAddressOf());
	__D3Dc()->GSSetShaderResources(2, 1, Tb.Read.GetAddressOf());
	__D3Dc()->PSSetShader(Ps.Get(), nullptr, 0);
	__D3Dc()->Draw((UINT)Vbdata->size(), 0);
	__D3Dc()->PSSetShader(nullptr, nullptr, 0);
	__D3Dc()->GSSetShader(nullptr, nullptr, 0);
	__D3Dc()->VSSetShader(nullptr, nullptr, 0);

}


