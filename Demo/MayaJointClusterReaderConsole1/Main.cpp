
#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>
#include <debugapi.h>

#include <maya\MItDependencyNodes.h>
#include <maya\MLibrary.h>
#include <maya\MFileIO.h>
#include <maya\MFileObject.h>
#include <maya\MGlobal.h>
#include <maya\MEulerRotation.h>
#include <maya\MString.h>
#include <maya\MVector.h>
#include <maya\MFnTransform.h>
#include <maya\MItDag.h>
#include <maya\MFnDagNode.h>
#include <maya\MFnMesh.h>
#include <maya\MPointArray.h>
#include <maya\MFloatVectorArray.h>
#include <maya\MStringArray.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya\MFnAnimCurve.h>
#include <maya\MFnIkJoint.h>
#include <maya\MFnSkinCluster.h>
#include <maya\MFnWeightGeometryFilter.h>
#include <maya\MDagPath.h>
#include <maya\MDagPathArray.h>
#include <maya\MItGeometry.h>

void OutputSkinCluster(MObject &object)
{
	MStatus status;
	MDagPathArray dagPaths;
	UINT32 dagPathsCount = 0;
	UINT32 geometriesCount = 0;
	UINT32 geometryIndex = 0;
	MFnSkinCluster cluster(object);

	std::cout << "Cluster=" << cluster.name().asChar() << std::endl;

	dagPathsCount = cluster.influenceObjects(dagPaths, &status);
	geometriesCount = cluster.numOutputConnections();

	// Loop though all geometries affected by this cluster...
	for (; geometryIndex < geometriesCount; geometryIndex++)
	{
		MDagPath dagPath;
		UINT32 outputIndex = 0;
		UINT32 influenceIndex = 0;

		// Get the DAG path of this geometry.
		cluster.indexForOutputConnection(geometryIndex, &status);
		cluster.getPathAtIndex(geometryIndex, dagPath);
		// Iterate though the components of this geometry.
		MItGeometry geometryIterator(dagPath);

		std::cout << " Skin=" << dagPath.partialPathName(&status).asChar() <<
			" Vertices=" << geometryIterator.count(&status) << std::endl;

		outputIndex = cluster.indexForOutputConnection(geometryIndex, &status);
		cluster.getPathAtIndex(outputIndex, dagPath);

		// Print influences names
		/*
		std::cout << "  Influences {" << dagPathsCount << "}" << std::endl << "   ";
		for (; influenceIndex < dagPathsCount; influenceIndex++)
			std::cout << " -> " << dagPaths[influenceIndex].partialPathName().asChar();
		std::cout << std::endl;
		*/

		UINT32 geometryComponentIndex = 0; // Ctrl index.
		for (; !geometryIterator.isDone(); geometryIterator.next(), geometryComponentIndex++)
		{
			MFloatArray weights;
			UINT32 weightIndex = 0;
			UINT32 influencesCount = 0;
			MObject geometryComponent = geometryIterator.component();
			cluster.getWeights(dagPath, geometryComponent, weights, influencesCount);

			std::cout << "    >> point " << geometryComponentIndex << 
				" is affected by " << weights.length() << " joints:" << std::endl;

			for (; weightIndex < weights.length(); weightIndex++)
			{
				std::cout << "    >>>> path=" << dagPaths[weightIndex].fullPathName().asChar() <<
					" [ " << weights[weightIndex] << " ]" << std::endl;
			}

		}

	}

}

void FindAllSkinClusters()
{
	MItDependencyNodes iterator(MFn::kSkinClusterFilter);

	for (; !iterator.isDone(); iterator.next())
	{
		MObject object = iterator.item();
		OutputSkinCluster(object);
	}

}


void OutputJointCluster(MObject &object)
{
	MFnWeightGeometryFilter geometryFilter(object);

	std::cout << "Filter " << geometryFilter.name().asChar() << std::endl;

}

void FindAllJointClusters()
{
	MItDependencyNodes iterator(MFn::kJointCluster);

	for (; !iterator.isDone(); iterator.next())
	{
		MObject object = iterator.item();
		OutputJointCluster(object);
	}

}

void PrintAll()
{
	MItDependencyNodes iterator(MFn::kInvalid);
	for (; !iterator.isDone(); iterator.next())
	{
		MObject object = iterator.item();
		std::cout << "Type " << object.apiTypeStr() << std::endl;
	}
}

int main(int argc, char *argv[])
{
	std::ofstream out("MeshInfo.dat");
	std::streambuf *coutbuf = std::cout.rdbuf();
	//std::cout.rdbuf(out.rdbuf());

	try
	{
		MStatus status;

		std::cout << "Initializing..." << std::endl;
		status = MLibrary::initialize(argv[0], true);
		if (!status)
		{
			std::cout << ">> Failed (" << status.errorString() << ")" << std::endl;
			return EXIT_FAILURE;
		}
		else
		{
			std::cout << ">> Succeeded" << std::endl;
		}

		BOOL result = FALSE;
		MFileObject fileObj;
		MString file = "D:\\Development\\Maya\\Scenes\\kinda-skinned-leg-posed-1.ma";
		status = fileObj.setRawFullName(file);
		result = fileObj.exists();

		if (!result)
		{
			std::cout.rdbuf(coutbuf);
			std::cout << "File not found. Exiting..." << std::endl;
			std::cout << "Press <Enter> to exit." << std::endl; getchar();
			return EXIT_FAILURE;
		}

		MString fileFullName = fileObj.rawFullName();

		std::cout << "Opening file..." << std::endl;
		status = MFileIO::open(fileFullName);

		if (status == MS::kSuccess)
		{
			std::cout << ">> Succeeded" << std::endl;

			//PrintAll();
			FindAllSkinClusters();
			FindAllJointClusters();

		}
		else
		{
			std::cout << ">> Failed (" << status.errorString() << ")" << std::endl;
		}

		// Maya API code goes here

		std::cout << "Cleaning..." << std::endl;
		MLibrary::cleanup(EXIT_SUCCESS, FALSE);
		std::cout << ">> Cleaned" << std::endl;
	}
	catch (...)
	{
		std::cout << "Exception" << std::endl;
	}

	//std::cout.rdbuf(coutbuf);

	std::cout << "Press <Enter> to exit." << std::endl; getchar();
	return EXIT_SUCCESS;

}