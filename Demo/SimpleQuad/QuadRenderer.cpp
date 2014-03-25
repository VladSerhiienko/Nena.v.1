#include "Framework.h"
#include "QuadRenderer.h"
#include <Nena\Assist.h>

void Framework::Application::QuadRendererNotify::OnDeviceLost() { }
void Framework::Application::QuadRendererNotify::OnDeviceRestored() { }

Framework::Application::QuadRenderer::QuadRenderer(Window *window) : LastError(S_OK)
{
	if (window) D3DResources = &window->Graphics;
	else LastError = E_POINTER;
	PendingChanges = FALSE;
}

::HRESULT Framework::Application::QuadRenderer::CreateDeviceDependentResources()
{
	using namespace Nena::Graphics::Resources;
	::HRESULT result = S_OK;

	Resources.VertexBuffer.Reset();
	Resources.IndexBuffer.Reset();
	Resources.TransformationBuffer.Reset();
	Resources.Vi.Reset();
	Resources.Vs.Reset();
	Resources.Ps.Reset();
	DSs.Reset();
	Rs.Reset();

	result = Assist::CreateVertexBuffer(
		GetD3DDevice(), Mesh.VerticesCount, 
		Mesh.Vertices, &Resources.VertexBuffer
		);
	__NENA_ReturnIfFailedByDef;
	result = Assist::CreateIndexBuffer(
		GetD3DDevice(), Mesh.IndicesCount, 
		Mesh.Indices, &Resources.IndexBuffer
		);
	__NENA_ReturnIfFailedByDef;
	result = Assist::LoadShader(
		GetD3DDevice(), L"Shaders/QuadVertexShader.cso", 
		Mesh.VSIEDesc, Mesh.VSIEDescCount, 
		&Resources.Vs, &Resources.Vi
		); 
	__NENA_ReturnIfFailedByDef;
	result = Assist::LoadShader(
		GetD3DDevice(), L"Shaders/QuadPixelShader.cso",
		&Resources.Ps
		);
	__NENA_ReturnIfFailedByDef;

	DepthStencilDescription dssd; 
	RasterizerStateDescription rsd; 
	ZeroValue(&dssd); Assist::Defaults(dssd);
	ZeroValue(&rsd); Assist::Defaults(rsd);

	//rsd.FrontCounterClockwise = TRUE;


	result = GetD3DDevice()->CreateDepthStencilState(&dssd, &DSs);
	__NENA_ReturnIfFailedByDef;
	result = GetD3DDevice()->CreateRasterizerState1(&rsd, &Rs);
	__NENA_ReturnIfFailedByDef;

	return result;

}

// there is no device independent resources
::HRESULT Framework::Application::QuadRenderer::CreateDeviceIndependentResources()
{
	::HRESULT result = S_OK;

	return result;
}

// constant buffer matrix (proj) should be updated
::HRESULT Framework::Application::QuadRenderer::CreateWindowSizeDependentResources()
{
	using namespace Nena::Graphics::Resources;
	::HRESULT result = S_OK;

	result = Assist::CreateConstantBuffer(
		GetD3DDevice(), &Mesh.Xform,
		&Resources.TransformationBuffer
		);
	__NENA_ReturnIfFailedByDef;

	PendingChanges = FALSE;

	return result;
}

::HRESULT Framework::Application::QuadRenderer::Update()
{
	using namespace Nena::Graphics::Resources;
	static ::HRESULT result = S_OK;
	static MappedSubresource mappedSr = { 0 };

	if (PendingChanges)
	{
		__MapWriteUnmap(
			GetD3DContext(), Resources.TransformationBuffer.Get(), 
			mappedSr, (HANDLE) &Mesh.Xform, result
			);

		PendingChanges = FALSE;
	}

	return result;
}

::HRESULT Framework::Application::QuadRenderer::Render()
{
	static ::HRESULT result = S_OK;

	GetD3DContext()->IASetVertexBuffers(0, 1, Resources.VertexBuffer.GetAddressOf(), &Mesh.VerticesStride, &Mesh.VerticesOffset);
	GetD3DContext()->IASetIndexBuffer(Resources.IndexBuffer.Get(), Mesh.IndexBufferFormat, Mesh.IndicesOffset);
	GetD3DContext()->IASetPrimitiveTopology(Mesh.Topology);
	GetD3DContext()->IASetInputLayout(Resources.Vi.Get());

	GetD3DContext()->VSSetShader(Resources.Vs.Get(), NULL, 0);
	GetD3DContext()->VSSetConstantBuffers(0, 1, Resources.TransformationBuffer.GetAddressOf());
	GetD3DContext()->PSSetShader(Resources.Ps.Get(), NULL, 0);
	//GetD3DContext()->RSSetState(Rs.Get());
	//GetD3DContext()->OMSetDepthStencilState(DSs.Get(), 1);

	GetD3DContext()->DrawIndexed(Mesh.IndicesCount, 0, 0);

	//GetD3DContext()->OMSetDepthStencilState(nullptr, 1);
	//GetD3DContext()->RSSetState(nullptr);
	GetD3DContext()->VSSetShader(nullptr, NULL, 0);
	GetD3DContext()->PSSetShader(nullptr, NULL, 0);

	return result;
}
