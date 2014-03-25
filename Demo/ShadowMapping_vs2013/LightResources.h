
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Light.h"

#ifndef __RASTERTEK_DEMO_LIGHTRESOURCES_INCLUDED__
#define __RASTERTEK_DEMO_LIGHTRESOURCES_INCLUDED__

namespace Demo
{
	struct LightResources
	{
		struct LightSpatialData
		{
			Nena::Matrix LightView;
			Nena::Matrix LightProj;
			Nena::Vector4 LightPosition;
		};
		struct LightDepthData
		{
			Nena::Matrix World;
			Nena::Matrix LightView;
			Nena::Matrix LightProj;
		};

		struct LightColorData
		{
			Nena::Color AmbientColor;
			Nena::Color DiffuseColor;
		};

		typedef ::Nena::Graphics::Resources::ConstantBuffer<LightSpatialData> LightSpatialConstBuffer;
		typedef ::Nena::Graphics::Resources::ConstantBuffer<LightColorData> LightColorConstBuffer;
		typedef ::Nena::Graphics::Resources::ConstantBuffer<LightDepthData> LightDepthConstBuffer;

		LightResources::LightColorData LightColorData;
		LightResources::LightDepthData LightDepthData;
		LightResources::LightSpatialData LightSpatialData;

		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;
		LightResources::LightColorConstBuffer LightColorBuffer;
		LightResources::LightDepthConstBuffer LightDepthBuffer;
		LightResources::LightSpatialConstBuffer LightSpatialBuffer;

		LightResources();
		~LightResources();

		void CreateDeviceResources();
		void Update(Nena::Matrix *focus, Demo::Light *light);

	};
}

#endif //!__RASTERTEK_DEMO_LIGHTRESOURCES_INCLUDED__