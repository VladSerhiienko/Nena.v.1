#pragma once

#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>

#include "Shape.h"

namespace Demo
{
	struct Camera;
	struct ShapeRenderer
	{
		typedef ::Nena::Graphics::Resources::ReadResource<::Nena::Graphics::Resources::Buffer> StructBuffer;
		typedef ::Nena::Graphics::Resources::Buffer ConstBuffer;
		typedef ::Nena::Graphics::Resources::Buffer VertexBuffer;

		struct TransformBuffer
		{
			::Nena::Matrix W, V, P;
		};

		Demo::Shape::FaceList *Vbdata = nullptr;
		Demo::Shape::Vector3List *Pbdata = nullptr;
		Demo::Shape::Vector3List *Nbdata = nullptr;
		Demo::Shape::Vector2List *Tbdata = nullptr;

		::Nena::Matrix *ShapeW = nullptr;
		Demo::Camera *Camera = nullptr;

		ShapeRenderer::TransformBuffer Xbdata;
		::Nena::Graphics::DeviceResources *D3DResources = nullptr;

		::Nena::Graphics::Resources::DepthStencil DSs;
		::Nena::Graphics::Resources::Rasterizer Rs;
		::Nena::Graphics::Resources::VSLayout Vi;
		::Nena::Graphics::Resources::VShader Vs;
		::Nena::Graphics::Resources::GShader Gs;
		::Nena::Graphics::Resources::PShader Ps;

		ShapeRenderer::VertexBuffer Vb;
		ShapeRenderer::StructBuffer Pb;
		ShapeRenderer::StructBuffer Nb;
		ShapeRenderer::StructBuffer Tb;
		ShapeRenderer::ConstBuffer Xb;

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
		void LoadShaders();

	};

}