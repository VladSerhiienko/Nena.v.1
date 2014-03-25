#pragma once

#include <Nena\Math.h>
#include <Nena\DeviceResources.h>

namespace Framework
{
	namespace Application
	{
		struct QuadMesh // contains all the stuff to create buffers
		{
			struct Transformation // WVP
			{
				Nena::Matrix World;
				Nena::Matrix View;
				Nena::Matrix Proj;
			};

			struct Vertex // vertex type 
			{
				Nena::Vector4 Position; // vertex position
				Nena::Vector4 Color; // vertex color
			};

			// allocates memory required and sets members to default values where possible
			QuadMesh(); 
			// deallocates used memory
			~QuadMesh(); 

			void GenerateVertices(); // generates vertices and indices (all set in members)
			void GenerateDefaultTransformation(); // generates matrices (see members)

			// required info

			Transformation Xform; // mesh transformation matrices wvp
			Nena::Vector3 Eye; // camera position point
			Nena::Vector3 Focus; // camera focus point
			::FLOAT FOV, AR, NP, FP; // field of view, aspect ration, near and far planes (z`s)

			Nena::Graphics::Resources::DxgiFormat IndexBufferFormat; // for indices buffer
			Nena::Graphics::Resources::VSInputElementDescription VSIEDesc[2];
			Nena::Graphics::Resources::PrimitiveTopology Topology;
			Vertex *Vertices; // points to vertices buffer info
			::UINT16 *Indices; // points to indices buffer info
			::UINT16 VerticesCount; // number of vertices
			::UINT16 IndicesCount; // number of indices
			::UINT16 VSIEDescCount; // number of vs input descriptions
			::BOOL FrontCounterClockwise; // for rasterizer state
			::HRESULT LastError; // is valid instance

			// additional info

			::UINT16 VerticesBytes; // vertices buffer bytesize
			::UINT16 IndicesBytes; // indices buffer bytesize
			::UINT32 VerticesStride; // vertices buffer stride
			::UINT32 VerticesOffset; // vertices buffer stride
			::UINT32 IndicesStride; // indices buffer stride
			::UINT32 IndicesOffset; // indices buffer stride
			::UINT16 XformBytes; // const xform buffer bytesize

		private:

			// internal impl detailes

			void AllocateMemory();
			void DeallocateMemory(); 

			HANDLE m_memoryHandle; // handle to allocated memory

		};
	}
}