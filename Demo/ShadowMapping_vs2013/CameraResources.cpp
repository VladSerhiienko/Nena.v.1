#include "CameraResources.h"

#include "Engine.h"

void Demo::CameraResources::CreateDeviceResources()
{
	using namespace Nena::Graphics;
	using namespace Nena::Graphics::Resources;

	auto engine = Engine::GetForCurrentThread();
	auto d3d = &engine->D3D;

	D3DCtx = d3d->Context.Get();
	ViewerBuffer.Create(d3d->Device.Get());
}

void Demo::CameraResources::Update(Nena::Matrix *world, Demo::Camera *camera)
{
	HRESULT result = S_OK;

	ViewerData.FocusWorld = (*world);
	ViewerData.CameraView = camera->View;
	ViewerData.CameraProj = camera->Projection;
	result = ViewerBuffer.SetData(D3DCtx, ViewerData);
}