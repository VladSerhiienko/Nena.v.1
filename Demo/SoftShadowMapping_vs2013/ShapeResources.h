
#include <Nena\Assist.h>
#include <Nena\DirectXTypes.h>
#include <Nena\DeviceResources.h>
#include <Nena\RenderTargetOverlay.h>

#include "Shape.h"
#include "Light.h"
#include "Transform.h"

#ifndef __RASTERTEK_DEMO_SHAPERESOURCES_INCLUDED__
#define __RASTERTEK_DEMO_SHAPERESOURCES_INCLUDED__

namespace Demo
{
	struct ShapeResources
	{
		typedef std::vector<Demo::ShapeResources*> Vector;

		typedef ::std::wstring String;
		typedef ::Nena::Graphics::Resources::Buffer VertexBuffer;
		typedef ::Nena::Graphics::Resources::SamplerState SamplerState;
		typedef ::Nena::Graphics::Resources::VSInputElementDescription PipelineInput[3];
		typedef ::Nena::Graphics::Resources::ReadResource<::Nena::Graphics::Resources::Buffer> StructBufferObject;
		typedef ::Nena::Graphics::Resources::ReadResource<::Nena::Graphics::Resources::Texture2> Texture2DObject;

		Demo::Shape *Shape;
		Demo::Light *Light;
		Demo::Camera *Viewer;

		Nena::Graphics::Resources::IDirect3DDevice *D3DDvc;
		Nena::Graphics::Resources::IDirect3DContext *D3DCtx;

		UINT32 Offset;
		UINT32 Stride;
		UINT32 VertexCount;
		ShapeResources::PipelineInput Input;			// IA  desc x 3
		ShapeResources::VertexBuffer Indices;			// IA  uint x 9
		ShapeResources::StructBufferObject Points;		// GS float x 3
		ShapeResources::StructBufferObject Normals;		// GS float x 3
		ShapeResources::StructBufferObject Texcoords;	// GS float x 2
		ShapeResources::Texture2DObject ColorTexture;	// PS float x 4

		ShapeResources();
		~ShapeResources();

		void CreateDeviceResources();
		static void GetInput(PipelineInput &input, UINT32 &stride);
	};

}

#endif // !__RASTERTEK_DEMO_SHAPERESOURCES_INCLUDED__
