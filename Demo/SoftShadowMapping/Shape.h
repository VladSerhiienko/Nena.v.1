#pragma once

#include <vector>
#include <Nena\Math.h>
#include "Transform.h"

namespace Demo
{
	struct Skeleton;
	struct SkinCluster;

	struct Shape
	{
		typedef unsigned __int32 UINT32, UINT;

		typedef struct VERTEX
		{

			Shape::UINT32 P, N, T;
			inline VERTEX(UINT32 v = 0) : P(v), N(v), T(v) { }
			inline VERTEX(VERTEX const &o) : P(o.P), N(o.N), T(o.T) { }

		} VTX, VERTEX32x3, *PVTX, Vertex;

		typedef struct FACE
		{
			typedef std::vector<FACE> Vector;
			typedef Shape::VTX TriangleIndices[3];

			TriangleIndices V;
			inline FACE() { V[0] = V[1] = V[2] = { 0 }; }
			inline FACE(FACE const &o) { V[0] = o.V[0]; V[1] = o.V[1]; V[2] = o.V[2]; }

		} FC, POLYGON32x3, *PFC, TRI, Face, Polygon, Triangle, Tri;


		typedef ::std::vector<::Nena::Vector3> Vector3List;
		typedef ::std::vector<::Nena::Vector2> Vector2List;
		typedef ::std::vector<Shape::FC> FaceList;

		Shape::Vector3List PointListOS;
		Shape::Vector3List NormalListOS;
		Shape::Vector2List TexcoordsList;
		Shape::Face::Vector PolygonList;
		UINT32 *N2PMap = nullptr;

		Demo::Transform TransformOS;
		Demo::Transform TransformWS;
		::Nena::Matrix TransformMWS;

		std::string Name;

		Shape();
		~Shape();

	private:

		Shape(Shape const &);

	};


}