#pragma once

#include <map>
#include <Nena\MayaReader.h>

namespace Demo
{
	struct Shape;
	struct Transform;

	struct ContentLoader
	{
		typedef ::BOOL Bool;
		typedef ::UINT32 UInt32;
		typedef ::std::string String;
		typedef ::Nena::Utility::MayaReader InternalImplementation;
		typedef InternalImplementation::Data MayaData;
		typedef MayaData::Shape MayaShape;
		typedef MayaData::Handle MayaHandle;
		typedef MayaData::Transform MayaTransform;
		typedef MayaData::SkinCluster::Skin MayaSkin;
		typedef ::std::map<::std::string, ContentLoader::MayaTransform*> MayaTransformMap;


		/*! \brief Loads shape data from file and fills out the provided instance.
		* \remark Provided instance should be barely created with no data inside.
		*/
		void UploadShape(
			_In_ String const &file,						//!< \param [in] file File path with shape & initial skeleton content
			_In_ String const &targetShape,					//!< \param [in] targetShape Shape node name \example "pCubeShape1"
			_In_ String const &targetShapeTransform,		//!< \param [in] targetShapeTransform Shape transform name \example "pCube1"
			_Notnull_ _Inout_ Shape *mesh					//!< \param [out] mesh Pointer reference to the shape data
			);

	private:

		ContentLoader::Bool IsJointWithSemantics(
			_In_ String const &jointName,
			_In_ String const &jointSematicName
			);

		void CopyTransform(
			_Notnull_ _Inout_ Transform *local,
			_Notnull_ _Inout_ Transform *world,
			_Notnull_ _In_ MayaTransform *source
			);

		InternalImplementation m_internalContentReader;

	};
}