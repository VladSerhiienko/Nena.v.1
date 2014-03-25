#pragma once

#include <map>
#include <Nena\MayaReader.h>

namespace Demo
{
	struct Mesh;
	struct Skeleton;
	struct Transform;

	struct Transform
	{
		::Nena::Quaternion Orientation;
		::Nena::Quaternion Rotation;
		::Nena::Vector3 Translation;
	};

	struct Skeleton
	{
		struct Joint
		{
			::std::string Name;
			::UINT32 Index = -1;
			Transform TransformOS;
			Transform TransformWS;
			Joint *Parent = nullptr;

#pragma region Member functions

			::Nena::Matrix GetTranslateWSM()
			{
				return ::Nena::XMMatrixTranslationFromVector(TransformWS.Translation);
			}

			::Nena::Matrix GetTranslateOSM()
			{
				return ::Nena::XMMatrixTranslationFromVector(TransformOS.Translation);
			}

			::Nena::Vector3 GetTranslateWSV()
			{
				return TransformWS.Translation;
			}

			::Nena::Vector3 GetTranslateOSV()
			{
				return TransformOS.Translation;
			}

			::Nena::Vector4 GetTranslateWSW()
			{
				return TransformWS.Translation;
			}

			::Nena::Vector4 GetTranslateOSW()
			{
				using namespace ::Nena;
				return TransformOS.Translation;
			}

			::Nena::Matrix GetRotateTransposeWSM()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
				rotate.Transpose();
				return rotate;
			}

			::Nena::Matrix GetRotateTransposeOSM()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformOS.Rotation);
				rotate.Transpose();
				return rotate;
			}

			::Nena::Matrix GetRotateWSM()
			{
				return ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
			}

			::Nena::Matrix GetRotateOSM()
			{
				return ::Nena::XMMatrixRotationQuaternion(TransformOS.Rotation);
			}

			::Nena::Quaternion GetRotateWSQ()
			{
				return TransformWS.Rotation;
			}

			::Nena::Quaternion GetRotateOSQ()
			{
				return TransformOS.Rotation;
			}

			::Nena::Matrix GetTransformMatrixWS()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
				::Nena::Matrix translate = ::Nena::XMMatrixTranslationFromVector(TransformWS.Translation);
				::Nena::Matrix transform = ::Nena::XMMatrixMultiply(rotate, translate);

				return transform;
			}

			::Nena::Matrix GetTransformMatrixInverseWS()
			{
				::Nena::Matrix rotate = ::Nena::XMMatrixRotationQuaternion(TransformWS.Rotation);
				rotate = ::Nena::XMMatrixTranspose(rotate);

				::Nena::Matrix translate = ::Nena::XMMatrixTranslationFromVector(
					::Nena::XMVectorNegate(
						::Nena::XMVector3Transform(TransformWS.Translation, rotate)
						)
					);

				::Nena::Matrix transform = ::Nena::XMMatrixMultiply(rotate, translate);

				/*::Nena::Matrix transform = GetTransformMatrixWS();
				transform = ::Nena::XMMatrixInverse(0, transform);*/

				return transform;
			}

			void UpdateTransformWS()
			{
				if (Parent)
				{
					Parent->UpdateTransformWS();

					Nena::Quaternion rotateWS =
						Nena::XMQuaternionMultiply(
						TransformOS.Rotation, Nena::XMQuaternionMultiply(
						TransformOS.Orientation, Parent->TransformWS.Rotation
						));

					Nena::Vector3 translateWS = Nena::XMVector3Rotate(
						TransformOS.Translation, Parent->TransformWS.Rotation
						);

					TransformWS.Translation = translateWS + Parent->TransformWS.Translation;
					TransformWS.Rotation = rotateWS;
				}
				else
				{
					Nena::Quaternion rotateWS = Nena::XMQuaternionMultiply(TransformOS.Rotation, TransformOS.Orientation);
					Nena::Vector3 translateWS = TransformOS.Translation;

					TransformWS.Translation = translateWS;
					TransformWS.Rotation = rotateWS;
				}

				std::cout << Name <<
					" X=" << TransformWS.Translation.x <<
					" Y=" << TransformWS.Translation.y <<
					" Z=" << TransformWS.Translation.z << std::endl;
			}

#pragma endregion

		};

		struct Cluster
		{
			struct Influence
			{
				Joint *Joint = nullptr;
				::FLOAT Weight = 0;
			};

			struct Component
			{
				::UINT32 Index = -1;
				::std::vector<Influence*> Influences;
			};

			::std::vector<Component*> Components;
			Mesh *Skin = nullptr;
		};

		::std::vector<Cluster *> SkinClusters;
		::std::vector<Joint *> Joints;
	};

	struct Mesh
	{
		struct Vertex
		{
			// Indices
			::UINT32 Point = -1, 
				Normal = -1, 
				Texcoords = -1;
		};

		struct Face
		{
			// Triangle
			Vertex Vertices[3]; 
		};

		::std::vector<::Nena::Vector3> PointsOS;
		::std::vector<::Nena::Vector3> PointsBP;
		::std::vector<::Nena::Vector3> NormalsOS;
		::std::vector<::Nena::Vector3> NormalsBP;

		::std::vector<::Nena::Vector3> Texcoords;
		::std::vector<Face> Polygons;

		Transform TransformOS;
		Transform TransformWS;
		Skeleton *SkeletonPosed = nullptr;
		Skeleton *SkeletonOrig = nullptr;

		void BindSkeleton()
		{
			Skeleton::Joint *joint;
			Skeleton::Joint *jointOrig;
			::UINT32 jointIndex = 0;
			::UINT32 jointCount = SkeletonOrig->Joints.size();
			Skeleton::Joint **joints = SkeletonPosed->Joints.data();
			Skeleton::Joint **jointsOrig = SkeletonOrig->Joints.data();
			Skeleton::Cluster *skinCluster = SkeletonOrig->SkinClusters.front();

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
					joint->GetRotateWSM(), joint->GetRotateTransposeWSM()
					);
			}

			::Nena::Vector3 pointOS;
			::Nena::Vector3 pointBP;
			::UINT32 pointIndex = 0;
			::UINT32 influenceIndex = 0;
			::UINT32 influenceCount = 0;
			Skeleton::Cluster::Influence *influence;
			::UINT32 pointCount = skinCluster->Components.size();

			for (pointIndex = 0; pointIndex < pointCount; pointIndex++)
			{
				influenceIndex = 0;
				influenceCount = skinCluster->Components[pointIndex]->Influences.size();
				Skeleton::Cluster::Influence **influences = skinCluster->Components[pointIndex]->Influences.data();

				influence = influences[influenceIndex]; 
				jointTransform = jointTransforms[influenceIndex] * influence->Weight;
				totalJointTransform = jointTransform;
				for (influenceIndex = 1; influenceIndex < influenceCount; influenceIndex++)
				{
					influence = influences[influenceIndex];
					jointTransform = jointTransforms[influenceIndex] * influence->Weight;
					totalJointTransform += jointTransform;
				}

				pointOS = PointsOS[pointIndex]; //pointOS.y += 5;
				pointBP = ::Nena::XMVector3Transform(pointOS, totalJointTransform);

				std::cout << pointIndex << "------------------------------------------------" << std::endl;
				std::cout << "<" << pointOS.x << " " << pointOS.y << " " << pointOS.z << "> => ";
				std::cout << "<" << pointBP.x << " " << pointBP.y << " " << pointBP.z << ">\n";

				PointsBP[pointIndex] = pointBP;
			}

		}

	};

	struct SceneFileContent
	{
		::std::vector<Mesh *> Shapes;
		Nena::Utility::MayaReader FileContentReader1;
		Nena::Utility::MayaReader FileContentReader2;

		void Demo::SceneFileContent::CopyTransform(
			Transform &object, Transform &world, 
			Nena::Utility::MayaReader::Data::Transform *src
			)
		{
			if (src->IsJoint) object.Orientation = src->RotateO;
			else object.Orientation = src->RotateOOS;
			if (src->IsJoint) world.Orientation = src->RotateO;
			else world.Orientation = src->RotateOOS;

			object.Translation = src->TranslateTS;
			object.Rotation = src->RotateQTS;
			world.Translation = src->TranslateWS;
			world.Rotation = src->RotateQWS;
		}

		typedef Nena::Utility::MayaReader::Data MayaData;
		typedef Nena::Utility::MayaReader::Data::Shape MayaShape;
		typedef Nena::Utility::MayaReader::Data::Transform MayaTransform;
		typedef Nena::Utility::MayaReader::Data::SkinCluster::Skin MayaSkin;
		typedef Nena::Utility::MayaReader::Data::Handle MayaHandle;
		typedef ::std::map<::std::string, MayaTransform*> MayaTransformMap;

		void CopySkeleton(
			Skeleton *skeleton,
			MayaSkin *mayaSkinObject,
			Nena::Utility::MayaReader &FileContentReader,
			MayaTransformMap &mayaTransformMap
			)
		{
			::std::map<::std::string, Skeleton::Joint *> jointMap;

			for (auto pair : mayaTransformMap)
			{
				if (pair.first[0] == 'j')
				{
					Skeleton::Joint *joint = new Skeleton::Joint;
					::INT32 jointIndex = ::std::stoi(pair.first.substr(5));
					joint->Index = jointIndex;
					joint->Name = pair.first;
					skeleton->Joints.push_back(joint);
					jointMap[pair.first] = joint;


					CopyTransform(joint->TransformOS, joint->TransformWS,
						pair.second
						);
				}
			}

			for (auto jointPair : jointMap)
			{
				for (auto node : FileContentReader.Root->GetKidsReference())
				if (node->GetNameReference() == jointPair.first && node->GetAdults()->size())
					jointPair.second->Parent = jointMap[node->GetAdults()->at(0)->GetNameCopy()];
			}

			Skeleton::Cluster *cluster = new Skeleton::Cluster;
			skeleton->SkinClusters.push_back(cluster);

			for (::UINT32 c = 0; c < mayaSkinObject->ComponentCount; c++)
			{
				Skeleton::Cluster::Component *component = new Skeleton::Cluster::Component;
				cluster->Components.push_back(component);
				component->Index = c;

				for (::UINT32 i = 0; i < mayaSkinObject->Components[c].InfluenceCount; i++)
				{
					::std::string jointName = mayaSkinObject->Components[c].Influences[i].PartialPath.substr(0);

					Skeleton::Cluster::Influence *influence = new Skeleton::Cluster::Influence;
					component->Influences.push_back(influence);

					influence->Weight = mayaSkinObject->Components[c].Influences[i].Weight;
					influence->Joint = jointMap[jointName];
				}
			}
		}

		void Demo::SceneFileContent::ReadFile(
			::std::string const &skinned,
			::std::string const &posed
			)
		{
			FileContentReader1.Import(skinned);

			::std::string targetShape = "pCubeShape1Orig";
			::std::string targetShapeTransform = "pCube1";
			::std::string targetClusterSkin = "pCubeShape1";

			// + all the joints

			MayaSkin *mayaSkinObject = nullptr;
			MayaShape *mayaShapeObject = nullptr;

			::std::map<::std::string, MayaTransform*> mayaTransformMap; // joints + shapes

			for (auto &handle : FileContentReader1.Root->GetHandlesReference())
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
					(handle->GetNameReference() == targetShapeTransform || 
					handle->GetNameReference()[0] == 'j'))
				{
					mayaTransformMap[handle->GetNameCopy()] = dynamic_cast<MayaTransform*>(handle);
				}
			}

			Mesh *cubeMesh = new Mesh;
			Shapes.push_back(cubeMesh);

			for (::UINT32 p = 0; p < mayaShapeObject->Points->at(1)->Count; p++)
				cubeMesh->PointsOS.push_back(mayaShapeObject->Points->at(1)->Vectors[p]);
			for (::UINT32 p = 0; p < mayaShapeObject->Points->at(1)->Count; p++)
				cubeMesh->PointsBP.push_back(mayaShapeObject->Points->at(1)->Vectors[p]);

			CopyTransform(
				cubeMesh->TransformOS, cubeMesh->TransformWS, 
				mayaTransformMap[targetShapeTransform]
				);

			cubeMesh->SkeletonOrig = new Skeleton;
			CopySkeleton(cubeMesh->SkeletonOrig, mayaSkinObject, FileContentReader1, mayaTransformMap);
			cubeMesh->SkeletonOrig->Joints.back()->UpdateTransformWS();

			mayaTransformMap.clear();
			FileContentReader2.Import(posed);
			for (auto &handle : FileContentReader2.Root->GetHandlesReference())
			{
				if (handle->GetType() == MayaData::IItem::kTransform &&
					handle->GetNameReference()[0] == 'j')
				{
					mayaTransformMap[handle->GetNameCopy()] = 
						dynamic_cast<MayaTransform*>(handle);
				}
			}

			cubeMesh->SkeletonPosed = new Skeleton;
			CopySkeleton(cubeMesh->SkeletonPosed, mayaSkinObject, FileContentReader2, mayaTransformMap);
			cubeMesh->SkeletonPosed->Joints.back()->UpdateTransformWS();

			mayaTransformMap.clear();
			mayaSkinObject = nullptr;
			mayaShapeObject = nullptr;

			cubeMesh->BindSkeleton();
		}

	};

}