#include "ContentLoader.h"

#include "Shape.h"
#include "Transform.h"

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
	_Notnull_ _In_ MayaTransform *source
	)
{
	if (source->IsJoint) object->OrientationQ = source->RotateO;
	else object->OrientationQ = source->RotateOOS;
	if (source->IsJoint) world->OrientationQ = source->RotateO;
	else world->OrientationQ = source->RotateOOS;

	object->Translation = source->TranslateTS;
	object->RotationQ = source->RotateQTS;
	world->Translation = source->TranslateWS;
	world->RotationQ = source->RotateQWS;
	world->RotationEA = source->RotateEA;
}

void Demo::ContentLoader::UploadShape(
	_In_ String const &file,
	_In_ String const &targetShape,
	_In_ String const &targetShapeTransform,
	_Notnull_ _Inout_ Shape *mesh

	)
{
	MayaShape *mayaShapeObject = nullptr;
	MayaTransform *mayaShapeTransformObject = nullptr;

	m_internalContentReader.Import(file);

	for (auto &handle : m_internalContentReader.Root->GetHandlesReference())
	{
		if (handle->GetNameReference() == targetShape &&
			handle->GetType() == MayaData::IItem::kShape)
		{
			mayaShapeObject = dynamic_cast<MayaShape*>(handle);
		}
		else if (handle->GetType() == MayaData::IItem::kTransform &&
			handle->GetNameReference() == targetShapeTransform)
		{
			mayaShapeTransformObject = dynamic_cast<MayaTransform*>(handle);
			break;
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
		mayaShapeTransformObject
		);
	m_internalContentReader.Clear();
}