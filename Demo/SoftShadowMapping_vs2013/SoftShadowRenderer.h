
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Shape.h"
#include "Engine.h"
#include "ShadowRenderer.h"

#ifndef __RASTERTEK_DEMO_SOFTSHADOWRENDERER_INCLUDED__
#define __RASTERTEK_DEMO_SOFTSHADOWRENDERER_INCLUDED__

namespace Demo
{
	struct SoftShadowRenderer
	{
		Demo::LightResources *Light; // shared with shadow
		Demo::CameraResources *Camera; // shared with shadow
		Demo::ShapeResources::Vector *Meshes; // shared with shadow

		Nena::Graphics::DeviceResources *D3D; // shared
		Nena::Graphics::Resources::IDirect3DDevice *D3DDvc; // shared
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx; // shared

		Nena::Graphics::Resources::VSLayout IL; // shared with shadow
		Nena::Graphics::Resources::VShader VS; // shared with shadow

		Nena::Graphics::Resources::GShader GS; // private
		Nena::Graphics::Resources::PShader PS; // private
		Nena::Graphics::Resources::SamplerState ClampPS; // private
		Nena::Graphics::Resources::SamplerState WrapPS; // private

		void HardValidation();
		void CreateDeviceResources(Demo::ShadowRenderer *shadow);

		void Update();
		void Render(Demo::ShadowRenderer *shadow);

	};
}

#endif // !__RASTERTEK_DEMO_SOFTSHADOWRENDERER_INCLUDED__
