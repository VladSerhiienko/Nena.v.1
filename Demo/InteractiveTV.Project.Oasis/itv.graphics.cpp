#include "app.precompiled.h"
#include "itv.graphics.h"


InteractiveTV::Graphics::Graphics() 
	: d3d(D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE)
	//: d3d(D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_REFERENCE)
{
}

InteractiveTV::Graphics::~Graphics()
{
}

void InteractiveTV::Graphics::Init()
{
	if (!Nena::App::GetForCurrentThread()->View.Raw) return;

	d3d.Vsync = TRUE;
	//d3d.SwapchainEffect = DXGI_SWAP_EFFECT_DISCARD;
	d3d.Windowed = !Nena::App::GetForCurrentThread()->View.Fullscreen;
	d2d.CreateDeviceResources(&d3d);

	d3d.SetWindow(Nena::App::GetForCurrentThread()->View.Raw);
	d2d.SetDpi(&d3d, d3d.Dpi.x, d3d.Dpi.y);
}
