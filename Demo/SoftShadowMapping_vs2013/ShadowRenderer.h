#pragma once

#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Shape.h"
#include "Engine.h"
#include "DepthRenderer.h"

#ifndef __RASTERTEK_SHADOWRENDERER_INCLUDED__
#define __RASTERTEK_SHADOWRENDERER_INCLUDED__

namespace Demo
{
	struct ShadowRenderer : public Nena::Graphics::DeviceRenderTargetBase
	{
		Demo::LightResources *Light; // shared with depth
		Demo::CameraResources *Camera; // shared with depth
		Demo::ShapeResources::Vector *Meshes; // shared with depth

		Nena::Graphics::DeviceResources *D3D; // shared
		Nena::Graphics::Resources::IDirect3DDevice *D3DDvc; // shared
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx; // shared

		Nena::Graphics::Resources::VSLayout IL; // shared with depth
		Nena::Graphics::Resources::VShader VS; // shared with depth

		Nena::Graphics::Resources::GShader GS; // private
		Nena::Graphics::Resources::PShader PS; // private
		Nena::Graphics::Resources::SamplerState ClampPS; // private
		Nena::Graphics::Resources::SamplerState WrapPS; // private

		Nena::Graphics::Resources::Texture2 RT;
		Nena::Graphics::Resources::Texture2 DS;
		Nena::Graphics::Resources::SRView RTR;
		Nena::Graphics::Resources::UAView RTW;
		ImageFilter *Filter;

		ShadowRenderer();
		~ShadowRenderer();

		void HardValidation();
		void CreateDeviceResources(DepthRenderer *depth);

		void Update();
		void Render(Demo::DepthRenderer *depth);
	};

}

#endif // !__RASTERTEK_SHADOWRENDERER_INCLUDED__