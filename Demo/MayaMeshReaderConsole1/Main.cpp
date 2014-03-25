
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

void OutputMeshFaces(MObject &object)
{
	MStatus status;
	MStringArray uvSets;
	MFnMesh mesh(object);

	status = mesh.getUVSetNames(uvSets);

	MItMeshPolygon polyIterator(object);
	BOOL uvs = uvSets.length() != 0 && mesh.numUVs() != 0;

	INT32 polyIndex = -1;
	while (!polyIterator.isDone())
	{
		INT32 count = (INT32)polyIterator.polygonVertexCount();

		std::cout << "   " "poly[" << ++polyIndex << "]" << std::endl;
		for (INT32 index = 0; index < count; index++)
		{
			std::cout << "     v=" << polyIterator.vertexIndex(index) <<
				" n=" << polyIterator.normalIndex(index); // << std::endl;

			if (uvs)
			{
				std::cout << " uvs= "; // << std::endl;
				INT32 uvIndex = 0;

				for (INT32 uvSetIndex = 0; uvSetIndex < (INT32)uvSets.length(); uvSetIndex++)
				{
					status = polyIterator.getUVIndex(index, uvIndex, &uvSets[uvSetIndex]);
					std::cout << "[s=" << uvSets[uvSetIndex].asChar() <<
						" i=" << uvIndex << "] "; // << std::endl;
				}

				std::cout << std::endl;
			}
		}

		polyIterator.next();
	}

}

void OutputMeshTextureCoords(MObject &object)
{
	MStatus status;
	MStringArray uvSets;
	MFnMesh mesh(object);

	status = mesh.getUVSetNames(uvSets);
	BOOL uvs = uvSets.length() != 0 && mesh.numUVs() != 0;

	if (!uvs)
	{
		std::cout << " NO TEXTURE COORDS " << std::endl;
		return;
	}

	for (INT32 uvSetIndex = 0; uvSetIndex < (INT32) uvSets.length(); uvSetIndex++)
	{
		MFloatArray u, v; mesh.getUVs(u, v, &uvSets[uvSetIndex]);

		INT32 count = mesh.numUVs(uvSets[uvSetIndex]);
		std::cout << "   uvset{" << count << "}=" << 
			uvSets[uvSetIndex].asChar() << std::endl;

		for (INT32 index = 0; index < count; index++)
			std::cout << "    uv[" << index << "]" << 
				" u=" << u[index] << " v=" << u[index] << 
				std::endl;
	}
}

void OutputMeshNormals(MObject &object)
{
	MStatus status;
	MFloatVectorArray nrmls;
	MFnMesh mesh(object);
	status = mesh.getNormals(nrmls, MSpace::kObject);

	for (UINT32 index = 0; index < nrmls.length(); index++)
	{
		std::cout << "   normal[" << index << //std::endl <<
			"] => nx=" << nrmls[index].x << //std::endl <<
			" ny=" << nrmls[index].y << //std::endl <<
			" nz=" << nrmls[index].z << std::endl;
	}
}

void OutputMeshPoints(MObject &object)
{
	MStatus status;
	MPointArray points;
	MFnMesh mesh(object);
	status = mesh.getPoints(points, MSpace::kObject);

	for (UINT32 index = 0; index < points.length(); index++)
	{
		std::cout << "   point[" << index << //std::endl <<
			"] => px=" << points[index].x << //std::endl <<
			" py=" << points[index].y << //std::endl <<
			" pz=" << points[index].z << std::endl;
	}
}

void OutputMeshInfo()
{
	MStatus status;
	MItDag dagMeshes(MItDag::kDepthFirst, MFn::kMesh);

	while (!dagMeshes.isDone())
	{
		MFnMesh mesh = dagMeshes.item(&status);
		if (status == MS::kSuccess) //mesh.isIntermediateObject())
		{
			std::cout << "  " "DAG mesh node = " << mesh.name().asChar() << std::endl;
			std::cout << "  " "DAG mesh vertices: " << std::endl;

			OutputMeshFaces(mesh.object());
			OutputMeshPoints(mesh.object());
			OutputMeshNormals(mesh.object());
			OutputMeshTextureCoords(mesh.object());
		}

		dagMeshes.next();
	}

}

void OutputParentInfo()
{
	MItDag dagIterator(MItDag::kDepthFirst);

	while (!dagIterator.isDone())
	{
		UINT32 index = 0;
		MFnDagNode node = dagIterator.item();
		MString nodeName = node.name();

		if (false) //!node.isIntermediateObject())
		{
			dagIterator.next();
			continue;
		}

		std::cout << "  " "DAG node = " << nodeName.asChar() << std::endl;
		//std::cout << "  " "DAG node kids count = " << node.childCount() << std::endl;

		for (index = 0; index < node.childCount(); index++)
		{
			MObject childObj = node.child(index);
			MFnDagNode childNode(childObj);
			std::cout << "   >>" "DAG child = " << childNode.name().asChar() << std::endl;

		}

		//std::cout << "  " "DAG node adults count = " << node.parentCount() << std::endl;

		for (index = 0; index < node.parentCount(); index++)
		{
			MObject adultObj = node.parent(index);
			MFnDagNode adultNode(adultObj);
			std::cout << "   <<" "DAG adult = " << adultNode.name().asChar() << std::endl;

		}

		dagIterator.next();
	}
}

void OutputTransform(MObject &object)
{
	if (object.hasFn(MFn::kTransform))
	{
		MFnDagNode dag(object);
		MFnTransform fn(object);
		MString name = fn.name();

		auto pc = dag.parentCount();
		auto cc = dag.childCount();

		std::cout << "  " ">> xform " << name.asChar() << 
			" cc=" << cc << " pc=" << pc << std::endl;

		MVector translation = fn.getTranslation(MSpace::kObject);
		MEulerRotation rotation; fn.getRotation(rotation);
		double scale[3]; fn.getScale(scale);

		std::cout << "    " ">> t x " << translation.x << std::endl;
		std::cout << "    " ">> t y " << translation.y << std::endl;
		std::cout << "    " ">> t z " << translation.z << std::endl;

		std::cout << "    " ">> r x " << rotation.x << std::endl;
		std::cout << "    " ">> r y " << rotation.y << std::endl;
		std::cout << "    " ">> r z " << rotation.z << std::endl;

		std::cout << "    " ">> s x " << scale[0] << std::endl;
		std::cout << "    " ">> s y " << scale[1] << std::endl;
		std::cout << "    " ">> s z " << scale[2] << std::endl;

	}
	else
	{
		std::cout << "\t" ">> xform NONE " << std::endl;
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
			status.perror("MLibrary::initialize");
			return (1);
		}
		else
		{
			std::cout << ">> Succeeded" << std::endl;
		}

		BOOL result = FALSE;
		MFileObject fileObj;
		MString file = "D:\\Development\\Maya\\Scenes\\scene1.ma";
		status = fileObj.setRawFullName(file);
		result = fileObj.exists();

		MString fileFullName = fileObj.rawFullName();

		std::cout << "Opening file..." << std::endl;
		status = MFileIO::open(fileFullName);

		if (status == MS::kSuccess)
		{
			std::cout << ">> Succeeded" << std::endl;

			OutputParentInfo();
			OutputMeshInfo();

			MItDependencyNodes nodeIterator(MFn::kTransform);
			while (!nodeIterator.isDone())
			{
				MObject node = nodeIterator.item();
				std::cout << "Node type = " << node.apiTypeStr() << std::endl;
				OutputTransform(node);
				nodeIterator.next();
			}
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