
#include "Skin.h"
#include <Nena\PlatformAssist.h>

void Demo::Skin::Prepare()
{
	PointListBP.clear();
	NormalListBP.clear();
	PointListBP.resize(Mesh->PointListOS.size());
	NormalListBP.resize(Mesh->NormalListOS.size());
}

void Demo::Skin::Alloc()
{
	if (CanHandleMeshOnDelete && !Mesh) Mesh = new Shape;
	if (CanHandleSkinClusterOnDelete && !Cluster) Cluster = new SkinCluster;
	if (CanHandlePosedSkeletonOnDelete && !Posed) Posed = new Skeleton;
	if (CanHandleOriginalSkeletonOnDelete && !Original) Original = new Skeleton;
}

void Demo::Skin::Pose(::FLOAT time)
{
	using namespace ::Nena;

	Platform::DebugTrace("Posing animated skeleton at %f \n", time);

	Posed->Root->Update(time);

	Skeleton::Joint *joint = 0;
	Skeleton::Joint *jointOrig = 0;
	Skeleton::Joint **joints = Posed->Joints.data();
	Skeleton::Joint **jointsOrig = Original->Joints.data();

	SkinCluster *skinCluster = Cluster;
	SkinCluster::Influence *influence = 0;
	SkinCluster::Influence **influences = 0;

	UINT32 jointIndex = 0;
	UINT32 pointIndex = 0;
	UINT32 normalIndex = 0;
	UINT32 influenceIndex = 0;
	UINT32 influenceCount = 0;
	UINT32 jointCount = (UINT32) Original->Joints.size();
	UINT32 pointCount = (UINT32) skinCluster->Components.size();
	UINT32 normalCount = (UINT32) NormalListBP.size();

	::Nena::Vector3 vectorOS;
	::Nena::Vector3 vectorBP;
	::Nena::Matrix jointTransform;
	::Nena::Matrix totalJointTransform;
	::Nena::Matrix *jointTransforms = new ::Nena::Matrix[jointCount];
	::Nena::Matrix *jointRotates = new ::Nena::Matrix[jointCount];

	for (jointIndex = 0; jointIndex < jointCount; jointIndex++)
	{
		joint = joints[jointIndex];
		jointOrig = jointsOrig[jointIndex];

		jointTransforms[jointIndex] = ::Nena::XMMatrixMultiply(
			jointOrig->GetTransformMatrixInverseWS(),
			joint->GetTransformMatrixWS()
			);

		jointRotates[jointIndex] = ::Nena::XMMatrixMultiply(
			jointOrig->GetRotateTransposeWSM(), joint->GetRotateWSM()
			);
	}

	for (pointIndex = 0; pointIndex < pointCount; pointIndex++)
	{
		influenceIndex = 0;
		influenceCount = (UINT32) skinCluster->Components[pointIndex]->Influences.size();
		influences = skinCluster->Components[pointIndex]->Influences.data();

		influence = influences[influenceIndex];
		jointTransform = jointTransforms[influenceIndex] * influence->Weight;
		totalJointTransform = jointTransform;

		for (influenceIndex = 1; influenceIndex < influenceCount; influenceIndex++)
		{
			influence = influences[influenceIndex];
			jointTransform = jointTransforms[influenceIndex] * influence->Weight;
			totalJointTransform += jointTransform;
		}

		vectorOS = Mesh->PointListOS[pointIndex];
		vectorBP = ::Nena::XMVector3Transform(vectorOS, totalJointTransform);

		/*Framework::Output(
			"point %2d => (%2.2f %2.2f %2.2f) : (%2.2f %2.2f %2.2f)\n", 
			pointIndex,
			vectorOS.x,
			vectorOS.y,
			vectorOS.z,
			vectorBP.x,
			vectorBP.y,
			vectorBP.z
			);*/

		PointListBP[pointIndex] = vectorBP;
	}

	for (normalIndex = 0, pointIndex = Mesh->N2PMap[normalIndex]; normalIndex < normalCount; normalIndex++)
	{
		influenceIndex = 0;
		influenceCount = (UINT32) skinCluster->Components[pointIndex]->Influences.size();
		influences = skinCluster->Components[pointIndex]->Influences.data();

		influence = influences[influenceIndex];
		jointTransform = jointRotates[influenceIndex] * influence->Weight;
		totalJointTransform = jointTransform;

		for (influenceIndex = 1; influenceIndex < influenceCount; influenceIndex++)
		{
			influence = influences[influenceIndex];
			jointTransform = jointRotates[influenceIndex] * influence->Weight;
			totalJointTransform += jointTransform;
		}

		vectorOS = Mesh->NormalListOS[normalIndex];
		vectorBP = ::Nena::XMVector3Transform(vectorOS, totalJointTransform);

		/*Framework::Output(
			"normal %2d => (%2.2f %2.2f %2.2f) : (%2.2f %2.2f %2.2f)\n",
			pointIndex,
			vectorOS.x,
			vectorOS.y,
			vectorOS.z,
			vectorBP.x,
			vectorBP.y,
			vectorBP.z
			);*/

		NormalListBP[normalIndex] = vectorBP;
	}

	delete[] jointTransforms;
	delete[] jointRotates;

}