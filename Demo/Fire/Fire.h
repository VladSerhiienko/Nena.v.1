
#include <Nena/Math.h>
#include <Nena/Assist.h>
#include <Nena/DeviceResources.h>

#include "Camera.h"

#ifndef __RASTERTEK_FIRE_INCLUDED__
#define __RASTERTEK_FIRE_INCLUDED__

//#define _Nena_Align16
#define _Nena_Align16 __declspec(align(16))

namespace Demo
{
	struct Engine;
	struct FeuerFrei
	{
		typedef Nena::Graphics::Resources::ReadResource<Nena::Graphics::Resources::Texture2> ReadTexture2D;

		struct Vertex
		{
			Nena::Vector4 Pos;
			Nena::Vector2 Tex;
		};

		struct _Nena_Align16 Float4x4x3
		{
			Nena::Matrix Packs[3];
		};

		struct _Nena_Align16 Float4x2
		{
			Nena::Vector4 Packs[2];

			inline void SetFrameTime(FLOAT v) { Packs[0].x = v; }
			inline void SetScrollX(FLOAT v) { Packs[0].y = v; }
			inline void SetScrollY(FLOAT v) { Packs[0].z = v; }
			inline void SetScrollZ(FLOAT v) { Packs[0].w = v; }
			inline void SetScaleX(FLOAT v) { Packs[1].x = v; }
			inline void SetScaleY(FLOAT v) { Packs[1].y = v; }
			inline void SetScaleZ(FLOAT v) { Packs[1].z = v; }
			inline void SetDistortion1X(FLOAT v) { Packs[0].x = v; }
			inline void SetDistortion1Y(FLOAT v) { Packs[0].y = v; }
			inline void SetDistortion2X(FLOAT v) { Packs[0].z = v; }
			inline void SetDistortion2Y(FLOAT v) { Packs[0].w = v; }
			inline void SetDistortion3X(FLOAT v) { Packs[1].x = v; }
			inline void SetDistortion3Y(FLOAT v) { Packs[1].y = v; }
			inline void SetDistortionScale(FLOAT v) { Packs[1].z = v; }
			inline void SetDistortionBias(FLOAT v) { Packs[1].w = v; }
		};

		Nena::Graphics::Resources::VSLayout InputIA;
		Nena::Graphics::Resources::Buffer VerticesIA;
		Nena::Graphics::Resources::Buffer IndicesIA;

		Nena::Graphics::Resources::VShader PipeVS;
		Nena::Graphics::Resources::ConstantBuffer<Float4x4x3> MatricesVS;
		Nena::Graphics::Resources::ConstantBuffer<Float4x2> NoiseVS;

		Nena::Graphics::Resources::PShader PipePS;
		Nena::Graphics::Resources::ConstantBuffer<Float4x2> DistortionPS;
		Nena::Graphics::Resources::SamplerState ClampPS;
		Nena::Graphics::Resources::SamplerState WrapPS;
		Nena::Graphics::Resources::RasterizerState RS;
		Nena::Graphics::Resources::BlendState BlendOM;

		FeuerFrei::ReadTexture2D AlphaPS;
		FeuerFrei::ReadTexture2D NoisePS;
		FeuerFrei::ReadTexture2D FirePS;

		//Nena::Graphics::Resources::DepthStencilState DssRS;
		//Nena::Graphics::Resources::BlendState BsRS;

		FeuerFrei::Float4x4x3 SpatialData;
		FeuerFrei::Float4x2 DistortionData;
		FeuerFrei::Float4x2 NoiseData;
		Demo::Camera Cam;

		FeuerFrei();
		void CreateResources();
		void CreateDeviceIndependentResources();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();

		void Update();
		void Render();

	};
}

#endif // !__RASTERTEK_FIRE_INCLUDED__
