#pragma once

#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Shape.h"

namespace Demo
{
	struct Camera;
	struct Light;
	struct ShapeRenderer
	{
		struct TransformBuffer // Gs
		{
			::Nena::Matrix W, V, P;
		};

		struct LightBuffer // Ps
		{
			::Nena::Color AC;
			::Nena::Color DC;
			::Nena::Vector4 D;
			::Nena::Vector4 S;
		};

		struct CameraBuffer
		{
			::Nena::Vector4 P;
		};

		typedef ::Nena::Graphics::Resources::ReadResource<::Nena::Graphics::Resources::Buffer> StructBufferObject;
		typedef ::Nena::Graphics::Resources::ReadResource<::Nena::Graphics::Resources::Texture2> Texture2DObject;
		typedef ::Nena::Graphics::Resources::ConstantBuffer<ShapeRenderer::TransformBuffer> TransformConstBufferObject;
		typedef ::Nena::Graphics::Resources::ConstantBuffer<ShapeRenderer::LightBuffer> LightConstBufferObject;
		typedef ::Nena::Graphics::Resources::ConstantBuffer<ShapeRenderer::CameraBuffer> CameraConstBufferObject;
		typedef ::Nena::Graphics::Resources::SamplerState SamplerState;
		typedef ::Nena::Graphics::Resources::Buffer VertexBuffer;
		typedef ::std::wstring String;

		Demo::Shape::FaceList *Vbdata = nullptr;
		Demo::Shape::Vector3List *Pbdata = nullptr;
		Demo::Shape::Vector3List *Nbdata = nullptr;
		Demo::Shape::Vector2List *Tbdata = nullptr;

		::Nena::Matrix *ShapeW = nullptr;
		Demo::Camera *Camera = nullptr;
		Demo::Light *Light = nullptr;

		::Nena::Graphics::DeviceResources *D3DResources = nullptr;
		::Nena::Graphics::OverlayResources *D2DResources = nullptr;

		::Nena::Graphics::Resources::DepthStencil DSs;
		::Nena::Graphics::Resources::Rasterizer Rs;
		::Nena::Graphics::Resources::VSLayout Vi;
		::Nena::Graphics::Resources::VShader Vs;
		::Nena::Graphics::Resources::GShader Gs;
		::Nena::Graphics::Resources::PShader Ps;

		ShapeRenderer::VertexBuffer Vb; // Ia
		ShapeRenderer::StructBufferObject Pb; // Gs
		ShapeRenderer::StructBufferObject Nb; // Gs
		ShapeRenderer::StructBufferObject Tb; // Gs

		ShapeRenderer::TransformConstBufferObject Xb; // Gs
		ShapeRenderer::CameraConstBufferObject Cb; // Gs
		ShapeRenderer::LightConstBufferObject Lb; // Ps
		ShapeRenderer::TransformBuffer Xbdata; // Gs
		ShapeRenderer::CameraBuffer Cbdata; // Gs
		ShapeRenderer::LightBuffer Lbdata; // Ps

		ShapeRenderer::Texture2DObject ShapeTexture; // Ps
		ShapeRenderer::SamplerState ShapeSampleType; // Ps
		ShapeRenderer::String ShapeTexturePath; // 16

		void CreateWindowSizeDependentResources();
		void CreateDeviceIndependentResources();
		void CreateDeviceDependentResources();
		void Update();
		void Render();

	private:

		inline ::Nena::Graphics::Resources::IDirect3DDevice *__D3Dd() { return D3DResources->Device.Get(); }
		inline ::Nena::Graphics::Resources::IDirect3DContext *__D3Dc() { return D3DResources->Context.Get(); }

		void UpdateBuffers();
		void LoadBuffers();
		void LoadTextures();
		void LoadSamplers();
		void LoadShaders();

	};

}