#include "LightResources.h"
#include "Engine.h"


Demo::LightResources::LightResources()
{
}

Demo::LightResources::~LightResources()
{
}

void Demo::LightResources::CreateDeviceResources()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	auto engine = Engine::GetForCurrentThread();
	auto d3d = &engine->D3D;

	D3DCtx = d3d->Context.Get();

	LightColorBuffer.Create(d3d->Device.Get());
	LightDepthBuffer.Create(d3d->Device.Get());
	LightSpatialBuffer.Create(d3d->Device.Get());
}

void Demo::LightResources::Update(Nena::Matrix *focus, Demo::Light *light)
{
	HRESULT result = S_OK;

	LightSpatialData.LightPosition.x = light->Displacement2.x;
	LightSpatialData.LightPosition.y = light->Displacement2.y;
	LightSpatialData.LightPosition.z = light->Displacement2.z;
	LightSpatialData.LightPosition.w = 1.0f;
	LightSpatialData.LightView = light->View;
	LightSpatialData.LightProj = light->Projection;

	LightDepthData.World = (*focus);
	LightDepthData.LightView = light->View;
	LightDepthData.LightProj = light->Projection;

	LightColorData.AmbientColor = light->AmbientColor;
	LightColorData.DiffuseColor = light->DiffuseColor;

	result = LightSpatialBuffer.SetData(D3DCtx, LightSpatialData);
	result = LightDepthBuffer.SetData(D3DCtx, LightDepthData);
	result = LightColorBuffer.SetData(D3DCtx, LightColorData);
}
