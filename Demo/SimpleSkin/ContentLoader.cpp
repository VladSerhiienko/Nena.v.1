#include "ContentLoader.h"

#include "Shape.h"
#include "Skeleton.h"
#include "Transform.h"
#include "SkinCluster.h"
#include "__DbgOut.h"

Demo::ContentLoader::Bool Demo::ContentLoader::IsJointWithSemantics(
	_In_ String const &jointName, _In_ String const &jointSematicName
	)
{
	return !jointName.compare(
		0, jointSematicName.size(), 
		jointSematicName
		);
}

void Demo::ContentLoader::CopyTransform(
	_Notnull_ _Inout_ Transform *object,
	_Notnull_ _Inout_ Transform *world,
	_Notnull_ _In_ MayaTransform *&source
	)
{
	if (source->IsJoint) object->Orientation = source->RotateO;
	else object->Orientation = source->RotateOOS;
	if (source->IsJoint) world->Orientation = source->RotateO;
	else world->Orientation = source->RotateOOS;

	object->Translation = source->TranslateTS;
	object->Rotation = source->RotateQTS;
	world->Translation = source->TranslateWS;
	world->Rotation = source->RotateQWS;
}

void Demo::ContentLoader::CopySkeleton(
	_Notnull_ _Inout_ Skeleton *&skeleton,
	_Maybenull_ _Inout_ SkinCluster *cluster,
	_Notnull_ _In_ MayaSkin *&mayaSkinObject,
	_Notnull_ _In_ MayaTransformMap *mayaTransformMap,
	_In_ String const &jointSematicName
	)
{
	SkeletonJointMap jointMap;

	for (auto pair : (*mayaTransformMap))
	{
		if (IsJointWithSemantics(pair.first, jointSematicName))
		{
			Skeleton::Joint *joint = new Skeleton::Joint;

			joint->Name = pair.first;
			skeleton->Joints.push_back(joint);
			jointMap[pair.first] = joint;

			CopyTransform(
				&joint->TransformOS, &joint->TransformWS,
				pair.second
				);
		}
	}

	for (auto jointPair : jointMap) for (auto node : m_internalContentReader.Root->GetKidsReference())
	{
		if (node->GetNameReference() == jointPair.first)
		{
			if (node->GetAdults()->size())
			{
				if (node->GetAdults()->size() == 1 && node->GetAdults()->at(0)->GetNameReference() == "world")
					skeleton->Root = (Skeleton::Handle*) jointPair.second;
				else jointPair.second->Parent = jointMap[node->GetAdults()->at(0)->GetNameCopy()];
			}

			if (jointPair.second->Parent) jointPair.second->Parent->Kids.push_back(jointPair.second);
		}
	}

	if (cluster)
	{
		::UINT32 localInfluenceCount;
		::std::string jointName;
		cluster->Components.clear();
		cluster->Components.resize(mayaSkinObject->ComponentCount);

		for (::UINT32 c = 0; c < mayaSkinObject->ComponentCount; c++)
		{
			SkinCluster::Component *component = new SkinCluster::Component;
			cluster->Components[c] = component;

			localInfluenceCount = 0;
			for (::UINT32 i = 0; i < mayaSkinObject->Components[c].InfluenceCount; i++)
			{
				//if (mayaSkinObject->Components[c].Influences[i].Weight < 0.001f) continue;

				SkinCluster::Influence *influence = new SkinCluster::Influence;
				component->Influences.push_back(influence);

				jointName = mayaSkinObject->Components[c].Influences[i].PartialPath.substr(0);
				influence->Weight = mayaSkinObject->Components[c].Influences[i].Weight;
				influence->Joint = jointMap[jointName];
				localInfluenceCount++;
			}

			MaxInfluenceCount = max(MaxInfluenceCount, localInfluenceCount);
		}
	}
}

void Demo::ContentLoader::UploadShape(
	_In_ String const &fileSkinned,
	_In_ String const &filePosed,
	_In_ String const &targetShape,
	_In_ String const &targetShapeTransform,
	_In_ String const &targetClusterSkin,
	_In_ String const &targetJointSematicName,
	_Notnull_ _Inout_ Shape *&mesh,
	_Notnull_ _Inout_ Skeleton *&original,
	_Notnull_ _Inout_ Skeleton *&posed,
	_Notnull_ _Inout_ SkinCluster *&cluster
	)
{

	MayaSkin *mayaSkinObject = nullptr;
	MayaShape *mayaShapeObject = nullptr;
	MayaTransformMap mayaTransformMap;

	m_internalContentReader.Import(fileSkinned);

	mayaTransformMap.clear();
	for (auto &handle : m_internalContentReader.Root->GetHandlesReference())
	{
		if (handle->GetNameReference() == targetShape &&
			handle->GetType() == MayaData::IItem::kShape)
		{
			mayaShapeObject = dynamic_cast<MayaShape*>(handle);
		}
		else if (handle->GetNameReference() == targetClusterSkin &&
			handle->GetType() == MayaData::IItem::kSkin)
		{
			mayaSkinObject = dynamic_cast<MayaSkin*>(handle);
		}
		else if (handle->GetType() == MayaData::IItem::kTransform &&
			(IsJointWithSemantics(handle->GetNameReference(), targetJointSematicName)) ||
			(handle->GetNameReference() == targetShapeTransform))
		{
			mayaTransformMap[handle->GetNameCopy()] = dynamic_cast<MayaTransform*>(handle);
		}
	}

	for (::UINT32 p = 0; p < mayaShapeObject->Points->at(1)->Count; p++)
		mesh->PointListOS.push_back(mayaShapeObject->Points->at(1)->Vectors[p]);
	for (::UINT32 n = 0; n < mayaShapeObject->Normals->at(1)->Count; n++)
		mesh->NormalListOS.push_back(mayaShapeObject->Normals->at(1)->Vectors[n]);
		for (::UINT32 t = 0; t < mayaShapeObject->Sets[0]->UVs.size(); t++)
		mesh->TexcoordsList.push_back(mayaShapeObject->Sets[0]->UVs[t]);

	for (::UINT32 f = 0; f < mayaShapeObject->Polygons.size(); f++)
	{
		Shape::FACE face; 

		for (::UINT32 v = 0; v < mayaShapeObject->Polygons[f]->VertexCount; v++)
		{
			face.V[v].P = mayaShapeObject->Polygons[f]->Vertices[v].PointIndex;
			face.V[v].N = mayaShapeObject->Polygons[f]->Vertices[v].NormalIndex;
			face.V[v].T = mayaShapeObject->Polygons[f]->Vertices[v].Texcoords[0].UVIndex;
		}

		mesh->PolygonList.push_back(face);
	}

	mesh->N2PMap = new UINT32[mayaShapeObject->Normals->at(1)->Count];
	for (::UINT32 n = 0; n < mayaShapeObject->Normals->at(1)->Count; n++)
	{
		::BOOL pointIndexFound = FALSE;

		for (::UINT32 f = 0; f < mayaShapeObject->Polygons.size() && !pointIndexFound; f++)
			for (::UINT32 v = 0; v < mayaShapeObject->Polygons[f]->VertexCount; v++)
				if (mayaShapeObject->Polygons[f]->Vertices[v].NormalIndex == n)
				{
					mesh->N2PMap[n] = mayaShapeObject->Polygons[f]->Vertices[v].PointIndex;

					/*Framework::Output("N%d ~ P%d\n",
						mayaShapeObject->Polygons[f].Vertices[v].NormalIndex,
						mayaShapeObject->Polygons[f].Vertices[v].PointIndex
						);*/

					pointIndexFound = TRUE;
					break;
				}

		_ASSERT_EXPR(pointIndexFound, L"Point index not found");
	}

	CopyTransform(
		&mesh->TransformOS, &mesh->TransformWS,
		mayaTransformMap[targetShapeTransform]
		);

	CopySkeleton(
		original, cluster, 
		mayaSkinObject, &mayaTransformMap, 
		targetJointSematicName
		);

	m_internalContentReader.Clear();
	m_internalContentReader.Import(filePosed);

	mayaTransformMap.clear();
	for (auto &handle : m_internalContentReader.Root->GetHandlesReference())
	{
		if (handle->GetType() == MayaData::IItem::kTransform &&
			IsJointWithSemantics(handle->GetNameReference(), targetJointSematicName))
		{
			mayaTransformMap[handle->GetNameCopy()] = dynamic_cast<MayaTransform*>(handle);
		}
	}

	CopySkeleton(
		posed, nullptr,
		mayaSkinObject, &mayaTransformMap, 
		targetJointSematicName
		);

	_ASSERT_EXPR(original->Joints.size() == posed->Joints.size(), L"Should be equal (different models?)");
	for (::UINT32 jointIndex = 0; jointIndex < posed->Joints.size(); jointIndex++)
	{
		_ASSERT_EXPR(original->Joints[jointIndex]->Name == posed->Joints[jointIndex]->Name, L"Should correspond (different models?)");
		posed->Joints[jointIndex]->Original = original->Joints[jointIndex];
	}

	mayaTransformMap.clear();
	mayaSkinObject = nullptr;
	mayaShapeObject = nullptr;

	m_internalContentReader.Clear();
}