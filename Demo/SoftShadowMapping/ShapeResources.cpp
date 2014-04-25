#include "Engine.h"
#include "ShapeResources.h"

#include <Nena\AppVars.h>
#include <Nena\Assist.h>
#include <Nena\PlatformAssist.h>

Demo::ShapeResources::ShapeResources()
{
	Offset = 0;
}

Demo::ShapeResources::~ShapeResources()
{
}

void Demo::ShapeResources::GetInput(PipelineInput &input, UINT32 &stride)
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	VSInputElementDescription inputDesc[3] =
	{
		{ "VSVERTEX0INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VSVERTEX1INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VSVERTEX2INDICES", 0, DXGI_FORMAT_R32G32B32_UINT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	}; 
	
	size_t copySize = (sizeof VSInputElementDescription) * ARRAYSIZE(inputDesc);
	memcpy(input, inputDesc, copySize);

	stride = sizeof Demo::Shape::Polygon;
}

void Demo::ShapeResources::CreateDeviceResources()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	auto engine = Engine::GetForCurrentThread();
	auto d3d = &engine->D3D;
	auto d2d = &engine->D2D;

	D3DDvc = d3d->Device.Get();
	D3DCtx = d3d->Context.Get();

	HRESULT result = S_OK;
	auto vars = Nena::App::Vars::GetForCurrentThread();
	if (!vars->GetRecordsCount()) result = vars->Read();

	GetInput(Input, Stride);
	VertexCount = (UINT32) Shape->PolygonList.size();

	result = Assist::CreateStructuredBuffer(D3DDvc,
		(UINT32) Shape->PointListOS.size(), 
		Shape->PointListOS.data(),
		Points.Rsrc.ReleaseAndGetAddressOf(),
		Points.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create buffer");
	result = Assist::CreateStructuredBuffer(D3DDvc,
		(UINT32) Shape->NormalListOS.size(), 
		Shape->NormalListOS.data(),
		Normals.Rsrc.ReleaseAndGetAddressOf(),
		Normals.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create buffer");
	result = Assist::CreateStructuredBuffer(D3DDvc,
		(UINT32) Shape->TexcoordsList.size(), 
		Shape->TexcoordsList.data(),
		Texcoords.Rsrc.ReleaseAndGetAddressOf(),
		Texcoords.Read.ReleaseAndGetAddressOf(),
		nullptr
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create buffer");
	result = Assist::CreateVertexBuffer(D3DDvc,
		(UINT32) Shape->PolygonList.size(), 
		Shape->PolygonList.data(),
		Indices.ReleaseAndGetAddressOf()
		);
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to create buffer");

	std::string recordName = "Demo/ShadowMapping/Scene/Import/" + Shape->Name + "/Texture";
	result = Assist::LoadTexture(d3d->Device.Get(), d2d->WICFactory.Get(),
		vars->GetRecord<Nena::App::Vars::String16>(recordName),
		ColorTexture.Rsrc.ReleaseAndGetAddressOf(),
		ColorTexture.Read.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(result), L"Failed to load texture");

}
