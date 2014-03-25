#pragma once

#include <map>
#include <Nena\MayaReader.h>
#include "Skeleton.h"

namespace Demo
{
	struct Shape;
	struct SkinCluster;
	struct Transform;

	struct ContentLoader
	{
		typedef ::UINT32 UInt32;
		typedef ::BOOL Bool;
		typedef ::std::string String;
		typedef ::Nena::Utility::MayaReader InternalImplementation;
		typedef ::std::map<::std::string, SkeletonJoint *> SkeletonJointMap;
		typedef InternalImplementation::Data MayaData;
		typedef MayaData::Shape MayaShape;
		typedef MayaData::Handle MayaHandle;
		typedef MayaData::Transform MayaTransform;
		typedef MayaData::SkinCluster::Skin MayaSkin;
		typedef ::std::map<::std::string, ContentLoader::MayaTransform*> MayaTransformMap;

		/*! \brief Loads shape and skeletons data from files and fills out the provided instances.  
		* \remark Provided instances should be barely created with no data inside.    
		* \remark Set field \c MinJointWeight to ensure everything is ok. 
		* \remark Get field \c MaxInfluenceCount when allocating fix-sized resources (textures, buffers or so).
		*/
		void UploadShape(
			_In_ String const &fileSkinned,					//!< \param [in] fileSkinned File path with shape & initial skeleton content
			_In_ String const &filePosed,					//!< \param [in] filePosed File path with deformed skeleton content
			_In_ String const &targetShape,					//!< \param [in] targetShape Shape node name \remark Usually ...Orig shape \example "pCubeShape1Orig"
			_In_ String const &targetShapeTransform,		//!< \param [in] targetShapeTransform Shape transform name \example "pCube1"
			_In_ String const &targetClusterSkin,			//!< \param [in] targetClusterSkin Skin cluster name \example "pCubeShape1"
			_In_ String const &targetJointSematicName,		//!< \param [in] targetJointSematicName Joint name pattern \remark First letters only \example "joint" to load "joint1",..,"joint9", etc
			_Notnull_ _Inout_ Shape *&mesh,					//!< \param [out] mesh Pointer reference to the shape data
			_Notnull_ _Inout_ Skeleton *&original,			//!< \param [out] original Pointer reference to the initial skeleton data
			_Notnull_ _Inout_ Skeleton *&posed,				//!< \param [out] posed Pointer reference to the deformed skeleton data
			_Notnull_ _Inout_ SkinCluster *&cluster			//!< \param [out] cluster Pointer reference to the shape influences by skeleton
			);

		//!< \var Minimal skeleton influence weight, that will be included into cluster component info
		::FLOAT MinJointWeight = 0.001f; 
		//!< \var Maximal skeleton influence count (can come in handy for GPU resource alloc)
		::UINT32 MaxInfluenceCount = 0;

	private:

		ContentLoader::Bool IsJointWithSemantics(
			_In_ String const &jointName,
			_In_ String const &jointSematicName
			);

		void CopyTransform(
			_Notnull_ _Inout_ Transform *local,
			_Notnull_ _Inout_ Transform *world,
			_Notnull_ _In_ MayaTransform *&source
			);

		void CopySkeleton(
			_Notnull_ _Inout_ Skeleton *&skeleton,
			_Maybenull_ _Inout_ SkinCluster *cluster,
			_Notnull_ _In_ MayaSkin *&mayaSkinObject,
			_Notnull_ _In_ MayaTransformMap *mayaTransformMap,
			_In_ String CONST &jointSematicName
			);

		InternalImplementation m_internalContentReader;

	};
}