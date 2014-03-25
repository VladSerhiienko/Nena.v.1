#pragma once

#include "Shape.h"
#include "Skeleton.h"
#include "SkinCluster.h"

#ifndef TRUE
 #define TRUE 1
#endif
#ifndef FALSE
 #define FALSE 0
#endif

namespace Demo
{
	struct Skin
	{
		typedef unsigned __int32 UINT32, UINT;
		typedef __int32 INT32, INT, BOOL;

		Shape::Vector3List PointListBP;
		Shape::Vector3List NormalListBP;

		SkinCluster *Cluster = nullptr;
		Skeleton *Original = nullptr;
		Skeleton *Posed = nullptr;
		Shape *Mesh = nullptr;

		Skin::BOOL CanHandleMeshOnDelete = TRUE;
		Skin::BOOL CanHandleOriginalSkeletonOnDelete = TRUE;
		Skin::BOOL CanHandlePosedSkeletonOnDelete = TRUE;
		Skin::BOOL CanHandleSkinClusterOnDelete = TRUE;

		void Alloc();
		void Prepare();
		void Pose();

		Skin() { }

		~Skin()
		{
			PointListBP.clear();
			NormalListBP.clear();
			if (CanHandleMeshOnDelete && Mesh) delete Mesh;
			if (CanHandleSkinClusterOnDelete && Cluster) delete Cluster;
			if (CanHandlePosedSkeletonOnDelete && Posed) delete Posed;
			if (CanHandleOriginalSkeletonOnDelete && Original) delete Original;
		}

	private:

		Skin(Skin const &);

	};
}