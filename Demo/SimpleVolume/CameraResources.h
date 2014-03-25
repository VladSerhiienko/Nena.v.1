#pragma once

#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Shape.h"
#include "Light.h"
#include "Transform.h"

#ifndef __RASTERTEK_DEMO_CAMERARESOURCES_INCLUDED__
#define __RASTERTEK_DEMO_CAMERARESOURCES_INCLUDED__

namespace Demo
{
	struct CameraResources
	{
		struct CameraData
		{
			Nena::Matrix FocusWorld;
			Nena::Matrix CameraView;
			Nena::Matrix CameraProj;
		};

		typedef ::Nena::Graphics::Resources::ConstantBuffer<CameraData> ViewerConstBuffer;

		CameraResources::CameraData ViewerData;
		CameraResources::ViewerConstBuffer ViewerBuffer;
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		void CreateDeviceResources();
		void Update(Nena::Matrix *focusWorld, Demo::Camera *camera);
	};
}

#endif