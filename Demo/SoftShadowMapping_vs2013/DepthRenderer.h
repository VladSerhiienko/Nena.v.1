
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "ShapeResources.h"
#include "CameraResources.h"
#include "LightResources.h"

#ifndef __RASTERTEK_DEMO_DEPTHRENDERER_INCLUDED__
#define __RASTERTEK_DEMO_DEPTHRENDERER_INCLUDED__

namespace Demo
{
	struct ImageFilter;
	struct DepthRenderer : public Nena::Graphics::DeviceRenderTargetBase
	{
		Demo::LightResources *Light;
		Demo::CameraResources *Camera;
		Demo::ShapeResources::Vector *Meshes;

		Nena::Graphics::Resources::IDirect3DDevice *D3DDvc;
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		Nena::Graphics::Resources::VSLayout IL;
		Nena::Graphics::Resources::VShader VS;
		Nena::Graphics::Resources::GShader GS;
		Nena::Graphics::Resources::PShader PS;
		Nena::Graphics::Resources::Texture2 RT;
		Nena::Graphics::Resources::Texture2 DS;
		Nena::Graphics::Resources::SRView RTR;
		Nena::Graphics::Resources::UAView RTW;

		DepthRenderer();
		~DepthRenderer();

		void HardValidation();
		void CreateDeviceResources();

		void Update();
		void Render();

	private:

		void CreateRendererResources();
		void CreateRenderTargetResources();

	};
};


#endif // __RASTERTEK_DEMO_DEPTHRENDERER_INCLUDED__
