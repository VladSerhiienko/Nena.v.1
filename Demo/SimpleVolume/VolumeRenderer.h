#pragma once

#include "Camera.h"
#include "Transform.h"

#include <Nena\Math.h>
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>

#ifndef __SIMPLEVOLUME_DEMO_VOLUME_INCLUDED__
#define __SIMPLEVOLUME_DEMO_VOLUME_INCLUDED__

namespace Demo
{
	struct VolumeResources
	{
		typedef float (*VolumeCallbackTy)(UINT32 i, UINT32 j, UINT32 k, UINT32 w, UINT32 h, UINT32 d);

		struct _declspec(align(16)) SpatialVs // vs cb0
		{
			Nena::Matrix World;
			Nena::Matrix CameraView;
			Nena::Matrix CameraProj;
		};

		struct _declspec(align(16)) CameraPs // ps cb0
		{
			Nena::Matrix CameraViewInverse;
			Nena::Matrix CameraProjection;
			Nena::Vector4 ScreenDims;
		};

		struct _declspec(align(16)) ObbPs // ps cb1
		{
			Nena::Matrix ObbWorld;
			Nena::Matrix ObbWorldInverse;
			Nena::Vector4 AabbMin;
			Nena::Vector4 AabbMax;
		};

		typedef Nena::Graphics::Resources::ActiveResource<Nena::Graphics::Resources::Texture3> ActiveTexture3;
		typedef Nena::Graphics::Resources::ConstantBuffer<SpatialVs> SpatialVsConstBuffer;
		typedef Nena::Graphics::Resources::ConstantBuffer<CameraPs> CameraPsConstBuffer;
		typedef Nena::Graphics::Resources::ConstantBuffer<ObbPs> ObbPsConstBuffer;
		typedef Nena::Graphics::Resources::Buffer VertexBuffer;

		Demo::Camera Viewer;
		ObbPs ObbPsData;
		CameraPs CameraPsData;
		SpatialVs SpatialVsData;
		Transform ObbTransform;

		UINT32 Width, Height, Depth;
		UINT32 ObbWidth, ObbHeight, ObbDepth;

		VertexBuffer ObbVsIndices;
		VertexBuffer ObbVsVertices;
		ActiveTexture3 ObbVsVolume;

		SpatialVsConstBuffer SpatialVsResource;
		ObbPsConstBuffer ObbPsResource;
		CameraPsConstBuffer CameraPsResource;
		Nena::Graphics::DeviceResources *D3D;
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		VolumeResources();

		void CreateDeviceResources(VolumeCallbackTy rule);
		void Update();

		static float CubeVolume2div3(UINT32 i, UINT32 j, UINT32 k, UINT32 w, UINT32 h, UINT32 d);
		static float SphereVolume2div3(UINT32 i, UINT32 j, UINT32 k, UINT32 w, UINT32 h, UINT32 d);

	};


	struct VolumeRenderer
	{
		Demo::VolumeResources Resource;

		Nena::Graphics::Resources::VSLayout Il;
		Nena::Graphics::Resources::VShader Vs;
		Nena::Graphics::Resources::PShader Ps;
		Nena::Graphics::Resources::SamplerState Ss;

		Nena::Graphics::DeviceResources *D3D;
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		void CreateDeviceResources(VolumeResources::VolumeCallbackTy rule = nullptr);
		void Update();
		void Render();

		static VolumeRenderer *Get()
		{
			static VolumeRenderer renderer;
			return &renderer;
		}

	};

}

#endif // !__SIMPLEVOLUME_DEMO_VOLUME_INCLUDED__
