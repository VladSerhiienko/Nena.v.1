
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



void OutputAnimCurve(MObject &object)
{
	MFnAnimCurve fn(object);

	UINT32 keyCount = fn.numKeys();
	if (!keyCount) return;

	std::cout << "   " /*3*/ << "Animcurve " << fn.name().asChar() << " {" << keyCount << "}" << std::endl;

	UINT32 keyIndex = 0;
	for (keyIndex = 0; keyIndex < keyCount; keyIndex++)
	{
		float time = (float) fn.time(keyIndex).as(MTime::kMilliseconds);
		float value = (float)fn.value(keyIndex);

		float m1x, m1y, m2x, m2y;
		fn.getTangent(keyIndex, m1x, m1y, TRUE);
		fn.getTangent(keyIndex, m2x, m2y, FALSE);

		MString tangent1;
		MFnAnimCurve::TangentType tangent1type = 
			fn.inTangentType(keyIndex);

		switch (tangent1type)
		{
		case MFnAnimCurve::TangentType::kTangentAuto: tangent1 = "kAuto"; break;
		case MFnAnimCurve::TangentType::kTangentLinear: tangent1 = "kLinear"; break;
		case MFnAnimCurve::TangentType::kTangentSmooth: tangent1 = "kSmooth"; break;
		case MFnAnimCurve::TangentType::kTangentFixed: tangent1 = "kFixed"; break;
		default: tangent1 = "UNKNOWN"; break;
		}

		MString tangent2;
		MFnAnimCurve::TangentType tangent2type = 
			fn.outTangentType(keyIndex);

		switch (tangent2type)
		{
		case MFnAnimCurve::TangentType::kTangentAuto: tangent2 = "kAuto"; break;
		case MFnAnimCurve::TangentType::kTangentLinear: tangent2 = "kLinear"; break;
		case MFnAnimCurve::TangentType::kTangentSmooth: tangent2 = "kSmooth"; break;
		case MFnAnimCurve::TangentType::kTangentFixed: tangent2 = "kFixed"; break;
		default: tangent2 = "UNKNOWN"; break;
		}

		std::cout << "      [" /*3+2=5*/ << keyIndex << "] time=" << time << " value=" 
			<< value  << " \n\t>> in  [x=" << m1x << " y=" << m1y << " t=" << tangent1 << 
			"] \n\t>> out [x=" << m1x << " y=" << m1y << " t=" << tangent2 << "]" << std::endl;
	}



}

BOOL OutputAnimCurveFromAttr(MObject &object, LPSTR attr)
{
	MStatus status;

	MPlug plug;
	INT32 index = 0;
	MPlugArray connections;
	MFnDependencyNode fn(object);

	plug = fn.findPlug(attr, &status);
	if (status != MS::kSuccess) return FALSE;

	plug.connectedTo(connections, TRUE, FALSE, &status);

	for (index = 0; index < connections.length(); index++)
	{
		MObject connectedObject = connections[index].node();

		if (connectedObject.hasFn(MFn::kAnimCurve))
		{
			OutputAnimCurve(connectedObject);
		}

	}

	return TRUE;
}

void PrintAnimCurveData()
{
	MStatus status;
	MItDependencyNodes nodeIterator(MFn::kJoint);
	while (!nodeIterator.isDone())
	{
		MObject node = nodeIterator.item();
		MFnIkJoint joint(node, &status);

		if (status == MS::kSuccess)
		{
			std::cout << "Scanning " << joint.name() << "..." << std::endl;

			OutputAnimCurveFromAttr(node, "tx");
			OutputAnimCurveFromAttr(node, "ty");
			OutputAnimCurveFromAttr(node, "tz");
			OutputAnimCurveFromAttr(node, "rx");
			OutputAnimCurveFromAttr(node, "ry");
			OutputAnimCurveFromAttr(node, "rz");
		}

		nodeIterator.next();
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
		MString file = "D:\\Development\\Maya\\Scenes\\joints-anim-1.ma";
		status = fileObj.setRawFullName(file);
		result = fileObj.exists();

		if (!result) return EXIT_FAILURE;

		MString fileFullName = fileObj.rawFullName();

		std::cout << "Opening file..." << std::endl;
		status = MFileIO::open(fileFullName);

		if (status == MS::kSuccess)
		{
			std::cout << ">> Succeeded" << std::endl;
			PrintAnimCurveData();
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