#include "MayaReader.h"
#include "Nena\rapidxml_utils.hpp"

::BOOL  Nena::Utility::HasEnding(std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length())
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	else return false;
}

::BOOL Nena::Utility::MayaReader::IsResultCheckedAt(::UINT32 const & result, ::UINT32 const &at = 0)
{
	return (result & (1 << at)) == (1 << at);
}

void Nena::Utility::MayaReader::GetUIntAttribute(XMLNode *node, ::std::string const &name, ::UINT32 &attribute, ::UINT32 &result)
{
	auto a = node->first_attribute(name.c_str());
	if (a) attribute = (::UINT32) std::stoul(a->value());
	result = a ? 1 : 0;
}

void Nena::Utility::MayaReader::GetFloatAttribute(XMLNode *node, ::std::string const &name, ::FLOAT &attribute, ::UINT32 &result)
{
	auto a = node->first_attribute(name.c_str());
	if (a) attribute = std::stof(a->value());
	result = a ? 1 : 0;
}

void Nena::Utility::MayaReader::GetStringAttribute(XMLNode *node, ::std::string const &name, ::std::string &attribute, ::UINT32 &result)
{
	auto a = node->first_attribute(name.c_str());
	if (a) attribute = a->value();
	result = a ? 1 : 0;
}

void Nena::Utility::MayaReader::GetXYZWAttributes(XMLNode *node, ::FLOAT &x, ::FLOAT &y, ::FLOAT &z, ::FLOAT *w, ::UINT32 &result)
{ 
	::UINT32 partialResult = 0;
	GetFloatAttribute(node, "X", x, partialResult); if (partialResult) result |= (1 << 0);
	GetFloatAttribute(node, "Y", y, partialResult); if (partialResult) result |= (1 << 1);
	GetFloatAttribute(node, "Z", z, partialResult); if (partialResult) result |= (1 << 2);
	if (w) GetFloatAttribute(node, "W", (*w), partialResult); if (partialResult) result |= (1 << 3);
}

void Nena::Utility::MayaReader::GetUVAttributes(XMLNode *node, ::FLOAT &u, ::FLOAT &v, ::UINT32 &result)
{
	::UINT32 partialResult = 0;
	GetFloatAttribute(node, "U", u, partialResult); if (partialResult) result |= (1 << 0);
	GetFloatAttribute(node, "V", v, partialResult); if (partialResult) result |= (1 << 1);
}

void Nena::Utility::MayaReader::GetCountAttribute(XMLNode *node, ::UINT32 &count, ::UINT32 &result)
{
	GetUIntAttribute(node, "Count", count, result);
}

void Nena::Utility::MayaReader::GetIndexAttribute(XMLNode *node, ::UINT32 &index, ::UINT32 &result)
{ 
	GetUIntAttribute(node, "Index", index, result);
}

void Nena::Utility::MayaReader::GetIndexCountAttributes(XMLNode *node, ::UINT32 &index, ::UINT32 &count, ::UINT32 &result)
{ 
	::UINT32 partialResult = 0;
	GetUIntAttribute(node, "Index", index, partialResult); if (partialResult) result |= (1 << 0);
	GetUIntAttribute(node, "Count", count, partialResult); if (partialResult) result |= (1 << 1);
}

void Nena::Utility::MayaReader::GetSpaceAttribute(XMLNode *node, ::std::string &space, ::UINT32 &result)
{
	GetStringAttribute(node, "Space", space, result);
}

void Nena::Utility::MayaReader::GetNameAttribute(XMLNode *node, ::std::string &name, ::UINT32 &result)
{ 
	GetStringAttribute(node, "Name", name, result);
}

void Nena::Utility::MayaReader::GetAdultCountAttribute(XMLNode *node, ::std::string &name, ::UINT32 &count, ::UINT32 &result)
{ 
	GetUIntAttribute(node, "Dag-Kid-Count", count, result);
}

void Nena::Utility::MayaReader::GetKidCountAttribute(XMLNode *node, ::std::string &name, ::UINT32 &count, ::UINT32 &result)
{ 
	GetUIntAttribute(node, "Dag-Adult-Count", count, result);
}

void Nena::Utility::MayaReader::GetAdultsNode(XMLNode *node, XMLNode *&adults)
{
	adults = node->first_node("Dag-Adults");
}

void Nena::Utility::MayaReader::GetKidsNode(XMLNode *node, XMLNode *&kids)
{
	kids = node->first_node("Dag-Kids");
}

void Nena::Utility::MayaReader::ImportFamily(
	XMLNode *hostNode,	Data::Node *hostData
	)
{
	::UINT32 result = 0;

	XMLNode *kidsNode; GetKidsNode(hostNode, kidsNode); if (kidsNode)
	{
		::UINT32 kidCount; GetCountAttribute(kidsNode, kidCount, result);
		XMLNode *kidNode = kidsNode->first_node();
		while (kidNode)
		{
			::std::string kidName; GetNameAttribute(kidNode, kidName, result);

			Data::Node* kidData = hostData->GetKidNode(kidName); if (!kidData)
			{
				kidData = Root->GetKidNode(kidName, TRUE);
				hostData->GetKids()->push_back(kidData);
				kidData->GetAdults()->push_back(hostData);
			}

			kidNode = kidNode->next_sibling();
		}
	}

	XMLNode *adultsNode; GetAdultsNode(hostNode, adultsNode); if (adultsNode)
	{
		::UINT32 adultCount; GetCountAttribute(adultsNode, adultCount, result);

		XMLNode *adultNode = adultsNode->first_node();
		while (adultNode)
		{
			::std::string adultName; GetNameAttribute(adultNode, adultName, result);
			Data::Node *adultData = hostData->GetAdultNode(adultName); if (!adultData)
			{
				adultData = Root->GetKidNode(adultName, TRUE);
				hostData->GetAdults()->push_back(adultData);
				adultData->GetKids()->push_back(hostData);
			}

			adultNode = adultNode->next_sibling();
		}
	}
}

void Nena::Utility::MayaReader::ImportSpacedVectors(
	XMLNode *shapeNode, Data::Shape::VectorsInSpaceCollection *&container, 
	::std::string const &collection, 
	::std::string const &item
	)
{
	::UINT32 result = 0;

	XMLNode *pointsNode = shapeNode->first_node(collection.c_str());

	if (pointsNode) container = new std::vector<Data::Shape::VectorsInSpace *>();

	while (pointsNode)
	{
		::UINT32 pointCount;
		::std::string pointsSpace;
		GetCountAttribute(pointsNode, pointCount, result);
		GetSpaceAttribute(pointsNode, pointsSpace, result);

		Data::Shape::VectorsInSpace *spaceVectors = new Data::Shape::VectorsInSpace();
		container->push_back(spaceVectors);

		spaceVectors->Vectors = new ::Nena::Vector3[pointCount];
		spaceVectors->Space = pointsSpace;
		spaceVectors->Count = pointCount;

		XMLNode *pointNode = pointsNode->first_node(item.c_str());

		while (pointNode)
		{
			::UINT32 index; GetIndexAttribute(pointNode, index, result);

			GetXYZWAttributes( pointNode,
				spaceVectors->Vectors[index].x,
				spaceVectors->Vectors[index].y,
				spaceVectors->Vectors[index].z,
				nullptr, result
				);

			pointNode = pointNode->next_sibling(item.c_str());
		}

		pointsNode = pointsNode->next_sibling(collection.c_str());
	}
}

void Nena::Utility::MayaReader::ImportPolygons(XMLNode *shapeNode, Data::Shape *shapeData)
{
	::UINT32 result = 0;
	::UINT32 bntnIndex = 0;
	::UINT32 polygonsCount = 0;
	XMLNode *polygonsNode = shapeNode->first_node("Polygons");
	GetCountAttribute(polygonsNode, polygonsCount, result);

	shapeData->Polygons.resize(polygonsCount);

	XMLNode *polygonNode = polygonsNode->first_node("Polygon");
	while (polygonNode)
	{
		::UINT32 vertexCount = 0; ::UINT32 polygonIndex;
		GetCountAttribute(polygonNode, vertexCount, result);
		GetIndexAttribute(polygonNode, polygonIndex, result);

		shapeData->Polygons[polygonIndex] = new Data::Shape::Polygon;
		shapeData->Polygons[polygonIndex]->Vertices = new Data::Shape::Polygon::Vertex[vertexCount];
		shapeData->Polygons[polygonIndex]->VertexCount = vertexCount;

		XMLNode *vertexNode = polygonNode->first_node("Vertex");
		while (vertexNode)
		{
			::UINT32 vertexIndex, pointIndex, normalIndex, texcoordsCount;
			GetIndexAttribute(vertexNode, vertexIndex, result);
			GetUIntAttribute(vertexNode, "Point-Index", pointIndex, result);
			GetUIntAttribute(vertexNode, "Normal-Index", normalIndex, result);
			GetCountAttribute(vertexNode, texcoordsCount, result);

			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].PointIndex = pointIndex;
			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].NormalIndex = normalIndex;
			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].BinormalIndex = bntnIndex;
			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].TangentIndex = bntnIndex;
			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].TexcoordCount = texcoordsCount;
			shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].Texcoords = new Data::Shape::Polygon::Vertex::Tex[texcoordsCount];

			::UINT32 texIndex = 0;
			XMLNode *texNode = vertexNode->first_node("Tex");
			while (texNode)
			{

				::std::string uvSetName; ::UINT32 uvIndex;
				GetStringAttribute(texNode, "UV-Set", uvSetName, result);
				GetUIntAttribute(texNode, "UV-Index", uvIndex, result);

				shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].Texcoords[texIndex].UVIndex = uvIndex;
				shapeData->Polygons[polygonIndex]->Vertices[vertexIndex].Texcoords[texIndex].UVSet = uvSetName;

				texNode = texNode->next_sibling("Tex");
			}

			vertexNode = vertexNode->next_sibling("Vertex");
			bntnIndex++;
		}

		polygonNode = polygonNode->next_sibling("Polygon");
	}
}

void Nena::Utility::MayaReader::ImportUVSets(XMLNode *shapeNode, Data::Shape *shapeData)
{
	::UINT32 result = 0;
	::UINT32 uvSetsCount = 0;
	XMLNode *uvSetsNode = shapeNode->first_node("UV-Sets");
	GetCountAttribute(uvSetsNode, uvSetsCount, result);

	shapeData->Sets.resize(uvSetsCount);

	if (uvSetsNode && uvSetsCount)
	{
		XMLNode *uvSetNode = uvSetsNode->first_node("UV-Set");
		while (uvSetNode)
		{
			::std::string uvSetName; 
			::UINT32 uvSetIndex;

			GetNameAttribute(uvSetNode, uvSetName, result);
			GetIndexAttribute(uvSetNode, uvSetIndex, result);

			::UINT32 uvCount = 0, bnCount = 0, tnCount = 0;

			GetUIntAttribute(uvSetNode, "UV-Count", uvCount, result);
			GetUIntAttribute(uvSetNode, "Binormal-Count", bnCount, result);
			GetUIntAttribute(uvSetNode, "Tangent-Count", tnCount, result);

			shapeData->Sets[uvSetIndex] = new Data::Shape::UVSet;
			shapeData->Sets[uvSetIndex]->Name = uvSetName;
			shapeData->Sets[uvSetIndex]->UVs.resize(uvCount);
			shapeData->Sets[uvSetIndex]->BNs.resize(bnCount);
			shapeData->Sets[uvSetIndex]->TNs.resize(tnCount);

			XMLNode *uvNode = uvSetNode->first_node("UV"); while (uvNode)
			{
				::UINT32 uvIndex;

				GetIndexAttribute(uvNode,
					uvIndex, result
					);
				GetUVAttributes(uvNode,
					shapeData->Sets[uvSetIndex]->UVs[uvIndex].x,
					shapeData->Sets[uvSetIndex]->UVs[uvIndex].y,
					result
					);

				uvNode = uvNode->next_sibling("UV");
			}

			XMLNode *bnNode = uvSetNode->first_node("Binormal"); while (bnNode)
			{
				::UINT32 bnIndex;

				GetIndexAttribute(bnNode,
					bnIndex, result
					);
				GetXYZWAttributes(bnNode,
					shapeData->Sets[uvSetIndex]->BNs[bnIndex].x,
					shapeData->Sets[uvSetIndex]->BNs[bnIndex].y,
					shapeData->Sets[uvSetIndex]->BNs[bnIndex].z,
					nullptr, result
					);

				bnNode = bnNode->next_sibling("Binormal");
			}

			XMLNode *tnNode = uvSetNode->first_node("Tangent"); while (tnNode)
			{
				::UINT32 tnIndex;

				GetIndexAttribute(tnNode,
					tnIndex, result
					);
				GetXYZWAttributes(tnNode,
					shapeData->Sets[uvSetIndex]->TNs[tnIndex].x,
					shapeData->Sets[uvSetIndex]->TNs[tnIndex].y,
					shapeData->Sets[uvSetIndex]->TNs[tnIndex].z,
					nullptr, result
					);

				tnNode = tnNode->next_sibling("Tangent");
			}

			uvSetNode = uvSetNode->next_sibling("UV-Set");
		}
	}
}

void Nena::Utility::MayaReader::ImportShape(XMLNode *shapeNode)
{
	::UINT32 result = 0;
	::std::string shapeName;
	GetNameAttribute(shapeNode, shapeName, result);

	Data::Node *shape = Root->GetKidNode(shapeName, TRUE);
	shape->m_type = Data::Node::kShape;
	shape->m_name = shapeName;

	Data::Shape *shapeData = new Data::Shape();
	Root->GetHandles()->push_back(shapeData);
	shape->m_handles.push_back(shapeData);
	shapeData->m_type = Data::Node::kShape;
	shapeData->m_name = shapeName;

	ImportFamily(shapeNode, shape);
	ImportSpacedVectors(shapeNode, shapeData->Points, "Points", "Point");
	ImportSpacedVectors(shapeNode, shapeData->Normals, "Normals", "Normal");
	ImportUVSets(shapeNode, shapeData);
	ImportPolygons(shapeNode, shapeData);
}

void Nena::Utility::MayaReader::ImportShapes()
{
	::UINT32 result = 0;

	XMLNode *shapesNode = m_dataNode->first_node("Shapes"); if (shapesNode)
	{
		::UINT32 shapeCount; 
		GetCountAttribute(shapesNode, shapeCount, result); if (IsResultCheckedAt(result))
		{
			XMLNode *shapeNode = shapesNode->first_node("Shape");
			while (shapeNode)
			{
				ImportShape(shapeNode);
				shapeNode = shapeNode->next_sibling("Shape");
			}
		}
	}
}

void Nena::Utility::MayaReader::ImportCurves(
	XMLNode *transformNode, Data::Transform *transformData
	)
{
	::UINT32 type = 0;
	::UINT32 result = 0;
	::UINT32 keyCount = 0;
	::std::string curveName;
	XMLNode *curveNode, *keyNode;

	curveNode = transformNode->first_node("Curve");
	if (curveNode) transformData->Curves = new ::std::vector<Data::Transform::Curve *>();

	while (curveNode)
	{
		GetCountAttribute(curveNode, keyCount, result);
		GetUIntAttribute(curveNode, "Type", type, result);
		GetStringAttribute(curveNode, "Name", curveName, result);

		Data::Transform::Curve *curveData = new Data::Transform::Curve();
		transformData->Curves->push_back(curveData);
		//Root->GetHandles()->push_back(curveData);

		curveData->m_name = curveName;
		curveData->m_type = Data::IItem::kAnimationCurve;
		curveData->Type = type;
		curveData->KeyCount = keyCount;
		curveData->Keys = new Data::Transform::Curve::Key[keyCount];

		keyNode = curveNode->first_node("Key");
		while (keyNode)
		{
			::UINT32 keyIndex;
			GetIndexAttribute(keyNode, keyIndex, result);

			GetFloatAttribute(keyNode, "Time", curveData->Keys[keyIndex].Time, result);
			GetFloatAttribute(keyNode, "Value", curveData->Keys[keyIndex].Value, result);
			GetFloatAttribute(keyNode, "InX", curveData->Keys[keyIndex].In.x, result);
			GetFloatAttribute(keyNode, "InY", curveData->Keys[keyIndex].In.y, result);
			GetFloatAttribute(keyNode, "OutX", curveData->Keys[keyIndex].Out.x, result);
			GetFloatAttribute(keyNode, "OutY", curveData->Keys[keyIndex].Out.y, result);
			GetUIntAttribute(keyNode, "InType", curveData->Keys[keyIndex].InTangent, result);
			GetUIntAttribute(keyNode, "OutType", curveData->Keys[keyIndex].OutTangent, result);

			keyNode = keyNode->next_sibling("Key");
		}

		curveNode = curveNode->next_sibling("Curve");
	}
}

void Nena::Utility::MayaReader::ImportTranslate(
	XMLNode *transformNode, Data::Transform *transformData
	)
{
	::UINT32 result = 0;
	::std::string space;

	XMLNode *translateNode = transformNode->first_node("Translate"); while (translateNode)
	{
		GetSpaceAttribute(translateNode, space, result);

		if (space == "Object") GetXYZWAttributes(
			translateNode,
			transformData->Translate.x,
			transformData->Translate.y,
			transformData->Translate.z,
			nullptr, result
			);
		else if (space == "Transform") GetXYZWAttributes(
			translateNode,
			transformData->TranslateTS.x,
			transformData->TranslateTS.y,
			transformData->TranslateTS.z,
			nullptr, result
			);
		else if (space == "World") GetXYZWAttributes(
			translateNode,
			transformData->TranslateWS.x,
			transformData->TranslateWS.y,
			transformData->TranslateWS.z,
			nullptr, result
			);

		translateNode = translateNode->next_sibling("Translate");
	}
}

void Nena::Utility::MayaReader::ImportRotate(
	XMLNode *transformNode, Data::Transform *transformData
	)
{
	::UINT32 result = 0;
	::std::string space;

	XMLNode *rotateNode = transformNode->first_node("Rotate");
	XMLNode *eulerNode = rotateNode->first_node("Euler-Angles");

	GetXYZWAttributes(eulerNode,
		transformData->RotateEA.x,
		transformData->RotateEA.y,
		transformData->RotateEA.z,
		nullptr, result
		);

	XMLNode *quat = rotateNode->first_node("Quaternion"); while (quat)
	{
		::std::string space; GetSpaceAttribute(quat, space, result);

		if (space == "Object") GetXYZWAttributes(
			quat,
			transformData->RotateQ.x,
			transformData->RotateQ.y,
			transformData->RotateQ.z,
			&transformData->RotateQ.w,
			result
			);
		else if (space == "Transform") GetXYZWAttributes(
			quat,
			transformData->RotateQTS.x,
			transformData->RotateQTS.y,
			transformData->RotateQTS.z,
			&transformData->RotateQTS.w,
			result
			);
		else if (space == "World") GetXYZWAttributes(
			quat,
			transformData->RotateQWS.x,
			transformData->RotateQWS.y,
			transformData->RotateQWS.z,
			&transformData->RotateQWS.w,
			result
			);

		quat = quat->next_sibling("Quaternion");
	}

	XMLNode *orient = rotateNode->first_node("Orientation"); while (orient)
	{
		::std::string space; GetSpaceAttribute(orient, space, result);

		if (space == "Object") GetXYZWAttributes(
			orient,
			transformData->RotateOOS.x,
			transformData->RotateOOS.y,
			transformData->RotateOOS.z,
			&transformData->RotateOOS.w,
			result
			);
		else if (space == "*Object") GetXYZWAttributes(
			orient,
			transformData->RotateO.x,
			transformData->RotateO.y,
			transformData->RotateO.z,
			&transformData->RotateO.w,
			result
			);
		else if (space == "Transform") GetXYZWAttributes(
			orient,
			transformData->RotateOTS.x,
			transformData->RotateOTS.y,
			transformData->RotateOTS.z,
			&transformData->RotateOTS.w,
			result
			);
		else if (space == "World") GetXYZWAttributes(
			orient,
			transformData->RotateOWS.x,
			transformData->RotateOWS.y,
			transformData->RotateOWS.z,
			&transformData->RotateOWS.w,
			result
			);

		orient = orient->next_sibling("Orientation");
	}


}

void Nena::Utility::MayaReader::ImportScale(
	XMLNode *transformNode, Data::Transform *transformData
	)
{
	::UINT32 result = 0;
	::std::string space;

	XMLNode *scaleNode = transformNode->first_node("Scale");
	//GetSpaceAttribute(scaleNode, space, result);

	GetXYZWAttributes(scaleNode,
		transformData->Scale.x,
		transformData->Scale.y,
		transformData->Scale.z,
		nullptr, result
		);
}

void Nena::Utility::MayaReader::ImportTransform(XMLNode *transformNode)
{
	::UINT32 result = 0;
	::std::string transformName;
	::UINT32 isJoint = 0;
	GetNameAttribute(transformNode, transformName, result);
	GetUIntAttribute(transformNode, "IsJoint", isJoint, result);

	Data::Node *transform = Root->GetKidNode(transformName, TRUE);
	transform->m_type = Data::Node::kTransform;
	transform->m_name = transformName;

	Data::Transform *transformData = new Data::Transform();
	transform->m_handles.push_back(transformData);
	Root->GetHandles()->push_back(transformData);

	transformData->IsJoint = result;
	transformData->m_type = Data::Node::kTransform;
	transformData->m_name = transformName;

	ImportFamily(transformNode, transform);
	ImportTranslate(transformNode, transformData);
	ImportRotate(transformNode, transformData);
	ImportScale(transformNode, transformData);
	ImportCurves(transformNode, transformData);
}

void Nena::Utility::MayaReader::ImportTransforms()
{
	::UINT32 result = 0;

	XMLNode *transformsNode = m_dataNode->first_node("Transforms"); if (transformsNode)
	{
		::UINT32 transformsCount; 
		GetCountAttribute(transformsNode, transformsCount, result); if (IsResultCheckedAt(result))
		{
			XMLNode *transformNode = transformsNode->first_node("Transform"); while (transformNode)
			{
				ImportTransform(transformNode);
				transformNode = transformNode->next_sibling("Transform");
			}
		}
	}
}

void Nena::Utility::MayaReader::ImportSkinCluster(XMLNode *clusterNode)
{
	::UINT32 result = 0;

	::std::string clusterName; GetNameAttribute(clusterNode, clusterName, result);
	::UINT32 skinCount; GetCountAttribute(clusterNode, skinCount, result);

	Data::SkinCluster *clusterData = new Data::SkinCluster();
	Root->GetHandles()->push_back(clusterData);
	clusterData->m_type = Data::IItem::kSkinCluster;
	clusterData->m_name = clusterName;

	XMLNode *skinNode = clusterNode->first_node("Skin"); while (skinNode)
	{
		Data::SkinCluster::Skin *skin = new Data::SkinCluster::Skin;
		clusterData->Geometries.push_back(skin);
		Root->GetHandles()->push_back(skin);

		::UINT32 skinIndex; GetIndexAttribute(skinNode, skinIndex, result);
		::std::string skinName; GetNameAttribute(skinNode, skinName, result);
		::UINT32 componentCount; GetCountAttribute(skinNode, componentCount, result);

		skin->m_name = skinName;
		skin->m_type = Data::IItem::kSkin;
		skin->ComponentCount = componentCount;
		skin->Components = new Data::SkinCluster::Skin::Component[componentCount];

		XMLNode *componentNode = skinNode->first_node("Component"); while (componentNode)
		{
			::UINT32 componentIndex; GetIndexAttribute(componentNode, componentIndex, result);
			::UINT32 influenceCount; GetCountAttribute(componentNode, influenceCount, result);

			skin->Components[componentIndex].InfluenceCount = influenceCount;
			skin->Components[componentIndex].Influences = new Data::SkinCluster::Skin::Component::Influence[influenceCount];

			XMLNode *influenceNode = componentNode->first_node("Influence"); while (influenceNode)
			{
				::UINT32 influenceIndex; GetIndexAttribute(influenceNode, influenceIndex, result);
				::FLOAT influenceWeight; GetFloatAttribute(influenceNode, "Weight", influenceWeight, result);
				::std::string fullPath; GetStringAttribute(influenceNode, "Full-Path", fullPath, result);
				::std::string partialPath; GetStringAttribute(influenceNode, "Partial-Path", partialPath, result);

				skin->Components[componentIndex].Influences[influenceIndex].Weight = influenceWeight;
				skin->Components[componentIndex].Influences[influenceIndex].FullPath = fullPath;
				skin->Components[componentIndex].Influences[influenceIndex].PartialPath = partialPath;

				influenceNode = influenceNode->next_sibling("Influence");
			}

			componentNode = componentNode->next_sibling("Component");
		}

		skinNode = skinNode->next_sibling("Skin");
	}
}

void Nena::Utility::MayaReader::ImportSkinClusters()
{
	::UINT32 result = 0;

	XMLNode *clustersNode = m_dataNode->first_node("Skin-Clusters"); if (clustersNode)
	{
		::UINT32 transformsCount; GetCountAttribute(clustersNode, transformsCount, result);

		if (IsResultCheckedAt(result))
		{
			XMLNode *clusterNode = clustersNode->first_node("Cluster");
			while (clusterNode)
			{
				ImportSkinCluster(clusterNode);

				clusterNode = clusterNode->next_sibling("Cluster");
			}
		}
	}
}

::BOOL Nena::Utility::MayaReader::Import(::std::string fileName)
{
	{
		FILE *f; errno_t err = ::fopen_s(&f, fileName.c_str(), "r");
		if (f)
		{
			OutputDebugStringA("MayaReader >> File ");
			OutputDebugStringA(fileName.c_str());
			OutputDebugStringA(" found.\n");
			fclose(f); 
		}
		else
		{
			OutputDebugStringA("MayaReader >> Cannot open file ");
			OutputDebugStringA(fileName.c_str());
			OutputDebugStringA(".\n");
			return FALSE;
		}
	}

	::rapidxml::file<> file(fileName.c_str());

	try
	{
		m_document.parse<::rapidxml::parse_default>(file.data());
	}
	catch (rapidxml::parse_error &error)
	{
		OutputDebugStringA("MayaReader >> Parse error \n");
		OutputDebugStringA(error.what());
		OutputDebugStringA(".\n");
		return FALSE;
	}

	m_dataNode = m_document.first_node();
	if (strcmp(m_dataNode->name(), "Data") != 0) 
	{
		OutputDebugStringA("MayaReader >> Format error.\n");
		m_document.clear(); 
		return FALSE; 
	}
	else
	{
		// Exporter="Nena.v.2013.v.2"
		auto exporter = m_dataNode->first_attribute("Exporter");
		if (exporter && strcmp(exporter->value(), "Nena.v.2013.v.2") != 0)
		{
			OutputDebugStringA("MayaReader >> Exporter version unsupported.\n");
			m_document.clear();
			return FALSE;
		}
	}

	if (!Root) Root = new Data::Node();

	ImportShapes();
	ImportTransforms();
	ImportSkinClusters();

	return TRUE;
}

#define __NENA_Free_pointer_memory(p) { if (p) { delete p; p = nullptr; } }
#define __NENA_Free_pointer_array_memory(p) { if (p) { delete [] p; p = nullptr; } }

void Nena::Utility::MayaReader::Clear()
{
	if (Root)
	{
		Data::ReleaseVector(Root->m_handles);
		Data::ReleaseVector(Root->m_kids);
		Data::ReleasePointer(Root);
		Root = nullptr;
	}

	m_document.clear();
}