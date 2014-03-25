
#include <Windows.h>
#include <debugapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <rapidxml.hpp>
#include <rapidxml_print.hpp>

extern "C"
{
	#include <Nena\Lua\lua.h>
	#include <Nena\Lua\lstate.h>
	#include <Nena\Lua\lualib.h>
	#include <Nena\Lua\lauxlib.h>
}

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
#include <maya\MAnimUtil.h>
#include <maya\MFnIkJoint.h>
#include <maya\MFnSkinCluster.h>
#include <maya\MFnWeightGeometryFilter.h>
#include <maya\MDagPath.h>
#include <maya\MDagPathArray.h>
#include <maya\MQuaternion.h>
#include <maya\MItGeometry.h>

namespace Lua
{
	class LuaState
	{
	public:

		lua_State *L;

		LuaState() { L = luaL_newstate(); luaL_openlibs(L); }
		~LuaState() { lua_close(L); }

		// get function
		template<typename T>
		T get(const char * varname) 
		{
			char temp[64];
			memset(temp, 0, sizeof(temp));
			int i = 0;
			int j = 0;
			int n = 0;
			while (varname[i] != '\0') {
				char c = varname[i];
				if (c == '.') {
					if (n == 0)
						lua_getglobal(L, temp);
					else
						lua_getfield(L, -1, temp);
					++n;
					memset(temp, 0, sizeof(temp));
					j = 0;

					if (lua_isnil(L, -1))
						return 0;
				}
				else {
					temp[j] = c;
					++j;
				}
				++i;
			}
			if (n == 0)
				lua_getglobal(L, temp);
			else
				lua_getfield(L, -1, temp);
			return lua_get<T>();
		}

		// Generic get
		template<typename T>
		T lua_get() {
			return 0;
		}

		// Specializations
		template <> float lua_get<float>() {
			return (float)lua_tonumber(L, -1);
		}
		template <> double lua_get<double>() {
			return lua_tonumber(L, -1);
		}
		template <> bool lua_get<bool>() {
			return (bool)lua_toboolean(L, -1);
		}
		template <> int lua_get<int>() {
			return (int)lua_tointeger(L, -1);
		}
		template <> unsigned int lua_get<unsigned int>() {
			return (unsigned int) lua_tonumber(L, -1);
		}
		template <> const char * lua_get<const char *>() {
			return lua_tostring(L, -1);
		}

		operator lua_State *()
		{
			return L;
		}
	};
}

static ::BOOL g_ExportShapes = 1;
static ::BOOL g_ExportJointClusters = 2;
static ::BOOL g_ExportSkinClusters = 4;
static ::BOOL g_ExportAnimationCurves = 8;
static ::BOOL g_ExportTransformations = 16;
static ::INT32 g_ExporterDo = 0;

static ::rapidxml::xml_document<> g_Out;
static ::rapidxml::xml_node<> *g_DataNode;
static ::rapidxml::xml_node<> *g_ShapesNode;
static ::rapidxml::xml_node<> *g_AnimationNode;
static ::rapidxml::xml_node<> *g_SkinClustersNode;
static ::rapidxml::xml_node<> *g_TransformationsNode;
static ::std::string g_OutPath; // output file
static ::std::string g_InPath; // input file
static ::std::string g_ConfigPath; // input file
static ::std::string g_App; // application
static ::MStatus g_MayaStatus; // validness

static ::TCHAR g_String[1024];

static ::Lua::LuaState g_Lua;

namespace Maya
{
	::BOOL Valid() 
	{ 
		return g_MayaStatus == MS::kSuccess; 
	}

	void Start()
	{
		std::cout << "Maya > Starting...\n";
		g_MayaStatus = MLibrary::initialize(g_App.c_str(), TRUE);
		std::cout << "Maya > Started.\n";

		if (!Valid())
		{
			std::cerr << "Maya: Failed to initialize library.\n";
			std::cerr << g_MayaStatus.errorString() << std::endl;
		}
	}

	void Clean()
	{
		std::cout << "Maya > Cleaning..." << std::endl;
		MLibrary::cleanup(EXIT_SUCCESS, FALSE);
		std::cout << "Maya > Cleaned." << std::endl;
	}

	void OpenFile()
	{
		std::cout << "Maya > Opening file...\n";
		g_MayaStatus = MFileIO::open(g_InPath.c_str());

		if (!Valid())
		{
			std::cerr << "Maya > Failed to open file.\n";
			std::cerr << g_MayaStatus.errorString() << std::endl;
			return;
		}
		else
		{
			std::cerr << "Maya > File opened.\n";
		}

		std::cout << std::endl;
	}
}

namespace Exporter
{
	void Start()
	{
		g_DataNode = g_Out.allocate_node(::rapidxml::node_element, "Data");
		g_DataNode->append_attribute(g_Out.allocate_attribute("Exporter", "Nena.v.2013.v.2"));
		g_DataNode->append_attribute(g_Out.allocate_attribute("Source", "Maya.v.2014.rev.196483"));
		g_Out.append_node(g_DataNode);
		Maya::Start();
		Maya::OpenFile();

		// ...
	}

	void Finalize()
	{
		Maya::Clean();
	}

	void AddFamily(::rapidxml::xml_node<> *&hostXmlNode, MFnDagNode &hostDagNode)
	{
		auto shapeKidsNode = g_Out.allocate_node(::rapidxml::node_element, "Dag-Kids");
		auto shapeKidsCount = g_Out.allocate_string(std::to_string(hostDagNode.childCount()).c_str());
		shapeKidsNode->append_attribute(g_Out.allocate_attribute("Count", shapeKidsCount));

		auto shapeAdultsNode = g_Out.allocate_node(::rapidxml::node_element, "Dag-Adults");
		auto shapeAdultsCount = g_Out.allocate_string(std::to_string(hostDagNode.parentCount()).c_str());
		shapeAdultsNode->append_attribute(g_Out.allocate_attribute("Count", shapeAdultsCount));

		hostXmlNode->append_attribute(g_Out.allocate_attribute("Dag-Kid-Count", shapeKidsCount));
		hostXmlNode->append_attribute(g_Out.allocate_attribute("Dag-Adult-Count", shapeAdultsCount));
		hostXmlNode->append_node(shapeKidsNode);
		hostXmlNode->append_node(shapeAdultsNode);

		if (hostDagNode.childCount())
		{
			for (::UINT32 index = 0; index < hostDagNode.childCount(); index++)
			{
				MFnDagNode dagNode = hostDagNode.child(index);
				auto dagNodeName = g_Out.allocate_string(dagNode.name().asChar());
				auto shapeNode = g_Out.allocate_node(::rapidxml::node_element, "Dag-Kid");
				shapeNode->append_attribute(g_Out.allocate_attribute("Name", dagNodeName));
				shapeKidsNode->append_node(shapeNode);
			}
		}

		if (hostDagNode.parentCount())
		{
			for (::UINT32 index = 0; index < hostDagNode.parentCount(); index++)
			{
				MFnDagNode dagNode = hostDagNode.parent(index);
				auto dagNodeName = g_Out.allocate_string(dagNode.name().asChar());
				auto shapeNode = g_Out.allocate_node(::rapidxml::node_element, "Dag-Adult");
				shapeNode->append_attribute(g_Out.allocate_attribute("Name", dagNodeName));
				shapeAdultsNode->append_node(shapeNode);
			}
		}
	}

	void AddControlPoints(::rapidxml::xml_node<> *&shapeNode, MFnMesh &shapeFn)
	{
		MStatus status;
		MPointArray points;
		status = shapeFn.getPoints(points, MSpace::kObject);
		auto pointsNode = g_Out.allocate_node(::rapidxml::node_element, "Points");
		auto pointsCount = g_Out.allocate_string(std::to_string(points.length()).c_str());
		pointsNode->append_attribute(g_Out.allocate_attribute("Space", "Object"));
		pointsNode->append_attribute(g_Out.allocate_attribute("Count", pointsCount));

		for (UINT32 index = 0; index < points.length(); index++)
		{
			auto pointNode = g_Out.allocate_node(::rapidxml::node_element, "Point");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(points[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(points[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(points[index].z).c_str());
			pointNode->append_attribute(g_Out.allocate_attribute("Index", i));
			pointNode->append_attribute(g_Out.allocate_attribute("X", x));
			pointNode->append_attribute(g_Out.allocate_attribute("Y", y));
			pointNode->append_attribute(g_Out.allocate_attribute("Z", z));

			pointsNode->append_node(pointNode);
		}

		shapeNode->append_node(pointsNode);

		status = shapeFn.getPoints(points, MSpace::kTransform);
		pointsNode = g_Out.allocate_node(::rapidxml::node_element, "Points");
		pointsCount = g_Out.allocate_string(std::to_string(points.length()).c_str());
		pointsNode->append_attribute(g_Out.allocate_attribute("Space", "Transform"));
		pointsNode->append_attribute(g_Out.allocate_attribute("Count", pointsCount));

		for (UINT32 index = 0; index < points.length(); index++)
		{
			auto pointNode = g_Out.allocate_node(::rapidxml::node_element, "Point");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(points[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(points[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(points[index].z).c_str());
			pointNode->append_attribute(g_Out.allocate_attribute("Index", i));
			pointNode->append_attribute(g_Out.allocate_attribute("X", x));
			pointNode->append_attribute(g_Out.allocate_attribute("Y", y));
			pointNode->append_attribute(g_Out.allocate_attribute("Z", z));

			pointsNode->append_node(pointNode);
		}

		shapeNode->append_node(pointsNode);

		status = shapeFn.getPoints(points, MSpace::kWorld);
		pointsNode = g_Out.allocate_node(::rapidxml::node_element, "Points"); 
		pointsCount = g_Out.allocate_string(std::to_string(points.length()).c_str());
		pointsNode->append_attribute(g_Out.allocate_attribute("Space", "World"));
		pointsNode->append_attribute(g_Out.allocate_attribute("Count", pointsCount));

		for (UINT32 index = 0; index < points.length(); index++)
		{
			auto pointNode = g_Out.allocate_node(::rapidxml::node_element, "Point");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(points[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(points[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(points[index].z).c_str());
			pointNode->append_attribute(g_Out.allocate_attribute("Index", i));
			pointNode->append_attribute(g_Out.allocate_attribute("X", x));
			pointNode->append_attribute(g_Out.allocate_attribute("Y", y));
			pointNode->append_attribute(g_Out.allocate_attribute("Z", z));

			pointsNode->append_node(pointNode);
		}

		shapeNode->append_node(pointsNode);
	}

	void AddNormals(::rapidxml::xml_node<> *&shapeNode, MFnMesh &shapeFn)
	{
		MStatus status;
		MFloatVectorArray normals;
		status = shapeFn.getNormals(normals, MSpace::kObject);
		auto normalsNode = g_Out.allocate_node(::rapidxml::node_element, "Normals");
		auto normalsCount = g_Out.allocate_string(std::to_string(normals.length()).c_str());
		normalsNode->append_attribute(g_Out.allocate_attribute("Space", "Object"));
		normalsNode->append_attribute(g_Out.allocate_attribute("Count", normalsCount));

		for (UINT32 index = 0; index < normals.length(); index++)
		{
			auto normalNode = g_Out.allocate_node(::rapidxml::node_element, "Normal");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(normals[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(normals[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(normals[index].z).c_str());
			normalNode->append_attribute(g_Out.allocate_attribute("Index", i));
			normalNode->append_attribute(g_Out.allocate_attribute("X", x));
			normalNode->append_attribute(g_Out.allocate_attribute("Y", y));
			normalNode->append_attribute(g_Out.allocate_attribute("Z", z));

			normalsNode->append_node(normalNode);
		}

		shapeNode->append_node(normalsNode);

		status = shapeFn.getNormals(normals, MSpace::kTransform);
		normalsNode = g_Out.allocate_node(::rapidxml::node_element, "Normals");
		normalsCount = g_Out.allocate_string(std::to_string(normals.length()).c_str());
		normalsNode->append_attribute(g_Out.allocate_attribute("Space", "Transform"));
		normalsNode->append_attribute(g_Out.allocate_attribute("Count", normalsCount));

		for (UINT32 index = 0; index < normals.length(); index++)
		{
			auto normalNode = g_Out.allocate_node(::rapidxml::node_element, "Normal");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(normals[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(normals[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(normals[index].z).c_str());
			normalNode->append_attribute(g_Out.allocate_attribute("Index", i));
			normalNode->append_attribute(g_Out.allocate_attribute("X", x));
			normalNode->append_attribute(g_Out.allocate_attribute("Y", y));
			normalNode->append_attribute(g_Out.allocate_attribute("Z", z));

			normalsNode->append_node(normalNode);
		}

		shapeNode->append_node(normalsNode);

		status = shapeFn.getNormals(normals, MSpace::kWorld);
		normalsNode = g_Out.allocate_node(::rapidxml::node_element, "Normals");
		normalsCount = g_Out.allocate_string(std::to_string(normals.length()).c_str());
		normalsNode->append_attribute(g_Out.allocate_attribute("Space", "World"));
		normalsNode->append_attribute(g_Out.allocate_attribute("Count", normalsCount));

		for (UINT32 index = 0; index < normals.length(); index++)
		{
			auto normalNode = g_Out.allocate_node(::rapidxml::node_element, "Normal");

			auto i = g_Out.allocate_string(std::to_string(index).c_str());
			auto x = g_Out.allocate_string(std::to_string(normals[index].x).c_str());
			auto y = g_Out.allocate_string(std::to_string(normals[index].y).c_str());
			auto z = g_Out.allocate_string(std::to_string(normals[index].z).c_str());
			normalNode->append_attribute(g_Out.allocate_attribute("Index", i));
			normalNode->append_attribute(g_Out.allocate_attribute("X", x));
			normalNode->append_attribute(g_Out.allocate_attribute("Y", y));
			normalNode->append_attribute(g_Out.allocate_attribute("Z", z));

			normalsNode->append_node(normalNode);
		}

		shapeNode->append_node(normalsNode);

	}

	void AddTextureCoordinates(::rapidxml::xml_node<> *&shapeNode, MFnMesh &mesh)
	{
		MStatus status; MStringArray uvSets;
		status = mesh.getUVSetNames(uvSets);

		auto uvsNode = g_Out.allocate_node(::rapidxml::node_element, "UV-Sets");
		auto uvsSetsCount = g_Out.allocate_string(std::to_string(uvSets.length()).c_str());
		uvsNode->append_attribute(g_Out.allocate_attribute("Count", uvsSetsCount));

		BOOL available = uvSets.length() != 0 && mesh.numUVs() != 0;

		if (!available)
		{
			uvsNode->append_attribute(g_Out.allocate_attribute("Available", "0"));
			return;
		}

		uvsNode->append_attribute(g_Out.allocate_attribute("Available", "1"));
		for (UINT32 uvSetIndex = 0; uvSetIndex < uvSets.length(); uvSetIndex++)
		{

			MFloatArray u, v; mesh.getUVs(u, v, &uvSets[uvSetIndex]);

			INT32 count = mesh.numUVs(uvSets[uvSetIndex]);

			auto uvsCount = g_Out.allocate_string(std::to_string(count).c_str());
			auto uvsIndex = g_Out.allocate_string(std::to_string(uvSetIndex).c_str());
			auto uvSetName = g_Out.allocate_string(uvSets[uvSetIndex].asChar());

			auto uvSetNode = g_Out.allocate_node(::rapidxml::node_element, "UV-Set");
			uvSetNode->append_attribute(g_Out.allocate_attribute("Name", uvSetName));
			uvSetNode->append_attribute(g_Out.allocate_attribute("Index", uvsIndex));
			uvSetNode->append_attribute(g_Out.allocate_attribute("UV-Count", uvsCount));

			for (INT32 index = 0; index < count; index++)
			{
				auto uvalue = g_Out.allocate_string(std::to_string(u[index]).c_str());
				auto vvalue = g_Out.allocate_string(std::to_string(v[index]).c_str());
				auto ivalue = g_Out.allocate_string(std::to_string(index).c_str());

				auto uvNode = g_Out.allocate_node(::rapidxml::node_element, "UV");
				uvNode->append_attribute(g_Out.allocate_attribute("Index", ivalue));
				uvNode->append_attribute(g_Out.allocate_attribute("U", uvalue));
				uvNode->append_attribute(g_Out.allocate_attribute("V", vvalue));

				uvSetNode->append_node(uvNode);
			}

			MFloatVectorArray tangents; mesh.getTangents(tangents, MSpace::kObject, &uvSets[uvSetIndex]);
			MFloatVectorArray binormals; mesh.getBinormals(binormals, MSpace::kObject, &uvSets[uvSetIndex]);
			auto tanCount = g_Out.allocate_string(std::to_string(tangents.length()).c_str());
			auto binCount = g_Out.allocate_string(std::to_string(binormals.length()).c_str());
			uvSetNode->append_attribute(g_Out.allocate_attribute("Binormal-Count", binCount));
			uvSetNode->append_attribute(g_Out.allocate_attribute("Tangent-Count", tanCount));

			for (UINT32 index = 0; index < binormals.length(); index++)
			{
				auto bin = binormals[index];
				auto x = g_Out.allocate_string(std::to_string(bin.x).c_str());
				auto y = g_Out.allocate_string(std::to_string(bin.y).c_str());
				auto z = g_Out.allocate_string(std::to_string(bin.z).c_str());
				auto i = g_Out.allocate_string(std::to_string(index).c_str());

				auto node = g_Out.allocate_node(::rapidxml::node_element, "Binormal");
				node->append_attribute(g_Out.allocate_attribute("Index", i));
				node->append_attribute(g_Out.allocate_attribute("X", x));
				node->append_attribute(g_Out.allocate_attribute("Y", y));
				node->append_attribute(g_Out.allocate_attribute("Z", z));
				uvSetNode->append_node(node);

			}

			for (UINT32 index = 0; index < tangents.length(); index++)
			{
				auto tan = tangents[index];
				auto x = g_Out.allocate_string(std::to_string(tan.x).c_str());
				auto y = g_Out.allocate_string(std::to_string(tan.y).c_str());
				auto z = g_Out.allocate_string(std::to_string(tan.z).c_str());
				auto i = g_Out.allocate_string(std::to_string(index).c_str());

				auto node = g_Out.allocate_node(::rapidxml::node_element, "Tangent");
				node->append_attribute(g_Out.allocate_attribute("Index", i));
				node->append_attribute(g_Out.allocate_attribute("X", x));
				node->append_attribute(g_Out.allocate_attribute("Y", y));
				node->append_attribute(g_Out.allocate_attribute("Z", z));
				uvSetNode->append_node(node);
			}

			uvsNode->append_node(uvSetNode);
		}

		shapeNode->append_node(uvsNode);
	}

	void AddFaces(::rapidxml::xml_node<> *&shapeNode, MFnMesh &mesh)
	{
		MStatus status;
		MStringArray uvSets;
		MItMeshPolygon polyIterator(mesh.object());

		status = mesh.getUVSetNames(uvSets);
		BOOL available = uvSets.length() != 0 && mesh.numUVs() != 0;

		auto facesNode = g_Out.allocate_node(::rapidxml::node_element, "Polygons");
		auto facesCount = g_Out.allocate_string(std::to_string(polyIterator.count()).c_str());
		facesNode->append_attribute(g_Out.allocate_attribute("Count", facesCount));

		INT32 faceIndex = 0;
		while (!polyIterator.isDone())
		{
			INT32 count = (INT32) polyIterator.polygonVertexCount();

			auto faceNode = g_Out.allocate_node(::rapidxml::node_element, "Polygon");
			auto polygonIndex = g_Out.allocate_string(std::to_string(faceIndex).c_str());
			auto vertexCount = g_Out.allocate_string(std::to_string(count).c_str());
			faceNode->append_attribute(g_Out.allocate_attribute("Index", polygonIndex));
			faceNode->append_attribute(g_Out.allocate_attribute("Count", vertexCount));
			facesNode->append_node(faceNode);
			++faceIndex;

			for (INT32 index = 0; index < count; index++)
			{
				auto vertexNode = g_Out.allocate_node(::rapidxml::node_element, "Vertex");

				auto polyIndex = g_Out.allocate_string(std::to_string(index).c_str());
				auto vertexIndex = g_Out.allocate_string(std::to_string(polyIterator.vertexIndex(index)).c_str());
				auto normalIndex = g_Out.allocate_string(std::to_string(polyIterator.normalIndex(index)).c_str());
				auto texCount = g_Out.allocate_string(std::to_string(uvSets.length()).c_str());
				vertexNode->append_attribute(g_Out.allocate_attribute("Index", polyIndex));
				vertexNode->append_attribute(g_Out.allocate_attribute("Point-Index", vertexIndex));
				vertexNode->append_attribute(g_Out.allocate_attribute("Normal-Index", normalIndex));
				vertexNode->append_attribute(g_Out.allocate_attribute("Count", texCount));

				faceNode->append_node(vertexNode);

				if (available)
				{
					INT32 uvIndex = 0;
					for (INT32 uvSetIndex = 0; uvSetIndex < (INT32) uvSets.length(); uvSetIndex++)
					{
						status = polyIterator.getUVIndex(index, uvIndex, &uvSets[uvSetIndex]);

						if (status == MS::kSuccess)
						{
							auto uvi = g_Out.allocate_string(std::to_string(index).c_str());
							auto uviNode = g_Out.allocate_node(::rapidxml::node_element, "Tex");
							auto uviSet = g_Out.allocate_string(uvSets[uvSetIndex].asChar());
							auto uviIndex = g_Out.allocate_string(std::to_string(uvIndex).c_str());
							uviNode->append_attribute(g_Out.allocate_attribute("UV-Set", uviSet));
							uviNode->append_attribute(g_Out.allocate_attribute("UV-Index", uviIndex));
							vertexNode->append_node(uviNode);
						}
					}


				}
			}

			polyIterator.next();
		}

		shapeNode->append_node(facesNode);
	}

	void ExportShape(MFnMesh &meshNode, MFnDagNode &dagMeshNode)
	{
		auto shapeNode = g_Out.allocate_node(::rapidxml::node_element, "Shape");

		std::cout << "Exporting > Shape (" << meshNode.name().asChar() << ")" << std::endl;

		shapeNode->append_attribute(g_Out.allocate_attribute("Name", meshNode.name().asChar()));

		AddFamily(shapeNode, dagMeshNode);
		AddControlPoints(shapeNode, meshNode);
		AddNormals(shapeNode, meshNode);
		AddTextureCoordinates(shapeNode, meshNode);
		AddFaces(shapeNode, meshNode);

		// ...

		g_ShapesNode->append_node(shapeNode);
	}

	void ExportShapes()
	{
		g_ShapesNode = g_Out.allocate_node(::rapidxml::node_element, "Shapes");
		std::cout << "Exporting > Shapes ..." << std::endl;

		g_DataNode->append_node(g_ShapesNode);

		::UINT32 shapesFound = 0;

		MStatus status1 = MS::kSuccess;
		MStatus status2 = MS::kSuccess;
		MItDag dagMeshIterator(MItDag::kDepthFirst, MFn::kMesh);

		while (!dagMeshIterator.isDone())
		{
			MFnMesh meshNode = dagMeshIterator.item(&status1);
			MFnDagNode dagMeshNode = dagMeshIterator.item(&status2);

			if (status1 == MS::kSuccess && status2 == MS::kSuccess)
			{
				ExportShape(meshNode, dagMeshNode);
				shapesFound++;
			}

			dagMeshIterator.next();
		}

		auto shapesCount = g_Out.allocate_string(std::to_string(shapesFound).c_str());
		std::cout << shapesFound << " Shapes exported" << std::endl << std::endl;

		g_ShapesNode->append_attribute(g_Out.allocate_attribute("Count", shapesCount));
	}

	void ExportSkinCluster(MFnSkinCluster &cluster, ::UINT32 index)
	{
		MStatus status;
		MDagPathArray dagPaths;

		::UINT32 dagPathsCount = cluster.influenceObjects(dagPaths, &status);
		::UINT32 geometriesCount = cluster.numOutputConnections();

		::LPSTR _clusterName = g_Out.allocate_string(cluster.name().asChar());
		::LPSTR _clusterIndex = g_Out.allocate_string(std::to_string(index).c_str());
		::LPSTR _skinCount = g_Out.allocate_string(std::to_string(geometriesCount).c_str());

		auto clusterNode = g_Out.allocate_node(::rapidxml::node_element, "Cluster");
		g_SkinClustersNode->append_node(clusterNode);
		clusterNode->append_attribute(g_Out.allocate_attribute("Name", _clusterName));
		clusterNode->append_attribute(g_Out.allocate_attribute("Index", _clusterIndex));
		clusterNode->append_attribute(g_Out.allocate_attribute("Count", _skinCount));

		std::cout << "Exporting > Cluster(" << cluster.name().asChar() << ")" << std::endl;

		::UINT32 geometryIndex = 0;
		for (; geometryIndex < geometriesCount; geometryIndex++)
		{
			MDagPath dagPath;
			::UINT32 outputIndex = 0;
			::UINT32 influenceIndex = 0;

			cluster.indexForOutputConnection(geometryIndex, &status);
			cluster.getPathAtIndex(geometryIndex, dagPath);

			MItGeometry geometryIterator(dagPath);

			outputIndex = cluster.indexForOutputConnection(geometryIndex, &status);
			cluster.getPathAtIndex(outputIndex, dagPath);

			::LPSTR _skinIndex = g_Out.allocate_string(std::to_string(geometryIndex).c_str());
			::LPSTR _skinName = g_Out.allocate_string(dagPath.partialPathName(&status).asChar());
			::LPSTR _vertexCount = g_Out.allocate_string(std::to_string(geometryIterator.count(&status)).c_str());

			auto skinNode = g_Out.allocate_node(::rapidxml::node_element, "Skin");
			clusterNode->append_node(skinNode);
			skinNode->append_attribute(g_Out.allocate_attribute("Name", _skinName));
			skinNode->append_attribute(g_Out.allocate_attribute("Index", _skinIndex));
			skinNode->append_attribute(g_Out.allocate_attribute("Count", _vertexCount));

			UINT32 geometryComponentIndex = 0; // Ctrl index.
			for (; !geometryIterator.isDone(); geometryIterator.next(), geometryComponentIndex++)
			{
				MFloatArray weights;
				::UINT32 influencesCount = 0;
				MObject geometryComponent = geometryIterator.component();
				cluster.getWeights(dagPath, geometryComponent, weights, influencesCount);

				::LPSTR _weightCount = g_Out.allocate_string(std::to_string(weights.length()).c_str());
				::LPSTR _pointIndex = g_Out.allocate_string(std::to_string(geometryComponentIndex).c_str());

				auto pointNode = g_Out.allocate_node(::rapidxml::node_element, "Component");
				skinNode->append_node(pointNode);
				pointNode->append_attribute(g_Out.allocate_attribute("Index", _pointIndex));
				pointNode->append_attribute(g_Out.allocate_attribute("Count", _weightCount));

				::UINT32 weightIndex = 0;
				for (; weightIndex < weights.length(); weightIndex++)
				{
					::LPSTR _weight = g_Out.allocate_string(std::to_string(weights[weightIndex]).c_str());
					::LPSTR _weightIndex = g_Out.allocate_string(std::to_string(weightIndex).c_str());
					::LPSTR _fullPath = g_Out.allocate_string(dagPaths[weightIndex].fullPathName().asChar());
					::LPSTR _partialPath = g_Out.allocate_string(dagPaths[weightIndex].partialPathName().asChar());

					auto influenceNode = g_Out.allocate_node(::rapidxml::node_element, "Influence");
					pointNode->append_node(influenceNode);
					influenceNode->append_attribute(g_Out.allocate_attribute("Index", _weightIndex));
					influenceNode->append_attribute(g_Out.allocate_attribute("Weight", _weight));
					influenceNode->append_attribute(g_Out.allocate_attribute("Partial-Path", _partialPath));
					influenceNode->append_attribute(g_Out.allocate_attribute("Full-Path", _fullPath));
				}
			}
		}
	}

	void ExportSkinClusters()
	{
		std::cout << "Exporting > Skin clusters ..." << std::endl;

		g_SkinClustersNode = g_Out.allocate_node(::rapidxml::node_element, "Skin-Clusters");
		g_DataNode->append_node(g_SkinClustersNode);

		::UINT32 clustersFound = 0;
		MItDependencyNodes iterator(MFn::kSkinClusterFilter);
		for (; !iterator.isDone(); iterator.next())
		{
			MObject object = iterator.item();
			if (object.hasFn(MFn::kSkinClusterFilter))
			{
				MFnSkinCluster cluster(object);
				ExportSkinCluster(cluster, clustersFound);
				clustersFound++;
			}
		}

		::LPSTR _clusterCount = g_Out.allocate_string(std::to_string(clustersFound).c_str());
		std::cout << clustersFound << " Skin clusters exported" << std::endl << std::endl;

		g_SkinClustersNode->append_attribute(g_Out.allocate_attribute("Count", _clusterCount));
	}

	void ExportAnimationCurve2(::rapidxml::xml_node<> *&transformationNode, MFnAnimCurve &fn)
	{
		::UINT32 keyIndex = 0;
		::UINT32 keyCount = fn.numKeys();
		::INT32 animCurveType = fn.animCurveType();

		auto _name = g_Out.allocate_string(fn.name().asChar());
		//auto _animCurveKind = g_Out.allocate_string("HermiteCurve");
		auto _keyCount = g_Out.allocate_string(std::to_string(keyCount).c_str());
		auto _animCurveType = g_Out.allocate_string(std::to_string(animCurveType).c_str());

		std::cout << "> Exporting > Curve (" << _name << "," << keyCount << ")" << std::endl;

		auto curveNode = g_Out.allocate_node(::rapidxml::node_element, "Curve");
		curveNode->append_attribute(g_Out.allocate_attribute("Count", _keyCount));
		curveNode->append_attribute(g_Out.allocate_attribute("Name", _name));
		curveNode->append_attribute(g_Out.allocate_attribute("Type", _animCurveType));
		//curveNode->append_attribute(g_Out.allocate_attribute("Kind", _animCurveKind));
		transformationNode->append_node(curveNode);

		for (keyIndex = 0; keyIndex < keyCount; keyIndex++)
		{
			::DOUBLE value = fn.value(keyIndex);
			::DOUBLE timeMss = fn.time(keyIndex).as(MTime::kMilliseconds);
			::INT32 in = (::INT32) fn.inTangentType(keyIndex);
			::INT32 out = (::INT32) fn.outTangentType(keyIndex);
			::FLOAT m1x, m1y, m2x, m2y;
			fn.getTangent(keyIndex, m1x, m1y, TRUE);
			fn.getTangent(keyIndex, m2x, m2y, FALSE);

			auto _time = g_Out.allocate_string(std::to_string(timeMss).c_str());
			auto _value = g_Out.allocate_string(std::to_string(value).c_str());
			auto _inx = g_Out.allocate_string(std::to_string(m1x).c_str());
			auto _iny = g_Out.allocate_string(std::to_string(m1y).c_str());
			auto _outx = g_Out.allocate_string(std::to_string(m2x).c_str());
			auto _outy = g_Out.allocate_string(std::to_string(m2y).c_str());
			auto _int = g_Out.allocate_string(std::to_string(in).c_str());
			auto _outt = g_Out.allocate_string(std::to_string(out).c_str());
			auto _index = g_Out.allocate_string(std::to_string(keyIndex).c_str());
			auto _unit = g_Out.allocate_string("Milliseconds");

			auto keyNode = g_Out.allocate_node(::rapidxml::node_element, "Key");

			keyNode->append_attribute(g_Out.allocate_attribute("Index", _index));
			keyNode->append_attribute(g_Out.allocate_attribute("Unit", _unit));
			keyNode->append_attribute(g_Out.allocate_attribute("Time", _time));
			keyNode->append_attribute(g_Out.allocate_attribute("Value", _value));
			keyNode->append_attribute(g_Out.allocate_attribute("InX", _inx));
			keyNode->append_attribute(g_Out.allocate_attribute("InY", _iny));
			keyNode->append_attribute(g_Out.allocate_attribute("OutX", _outx));
			keyNode->append_attribute(g_Out.allocate_attribute("OutY", _outy));
			keyNode->append_attribute(g_Out.allocate_attribute("InType", _int));
			keyNode->append_attribute(g_Out.allocate_attribute("OutType", _outt));

			curveNode->append_node(keyNode);
		}
	}

	void ExportAnimationCurves2(::rapidxml::xml_node<> *&transformationNode, MDagPath &transformPath, ::UINT32 &curvesFound)
	{
		curvesFound = 0;

		MStatus status;
		MPlugArray animatedPlugs;
		UINT32 plugIndex, curveIndex;
		MAnimUtil::findAnimatedPlugs(transformPath, animatedPlugs, FALSE, &status);

		if (status == MS::kSuccess)
		{
			for (plugIndex = 0; plugIndex < animatedPlugs.length(); plugIndex++)
			{
				MObjectArray curveObjects;
				MAnimUtil::findAnimation(animatedPlugs[plugIndex], curveObjects, &status);

				if (status == MS::kSuccess)
				{
					for (curveIndex = 0; curveIndex < curveObjects.length(); curveIndex++)
					{
						MFnAnimCurve curveFn(curveObjects[curveIndex], &status);

						if (status == MS::kSuccess)
						{
							ExportAnimationCurve2(transformationNode, curveFn);
							curvesFound++;
						}
					}
				}
			}
		}
	}

	void ExportTransformation2(::rapidxml::xml_node<> *&hostXmlNode, MObject &object, ::rapidxml::xml_node<> **transformNodeAddress)
	{
		MStatus status;
		MFnIkJoint joint(object, &status);
		::BOOL isJoint = status == MS::kSuccess;

		MFnTransform fn(object);
		MFnDagNode dagNode(object);

		MString name = fn.name();

		std::cout << "Exporting > Transform (" << name.asChar() << ")" << std::endl;

		double3 scale;
		MEulerRotation rotation;

		MQuaternion rotateQuat;
		MQuaternion rotateQuatTS;
		MQuaternion rotateQuatWS;

		MQuaternion rotateOrient;
		MQuaternion rotateOrientOS;
		MQuaternion rotateOrientTS;
		MQuaternion rotateOrientWS;

		MVector translation;
		MVector translationTS;
		MVector translationWS;

		if (isJoint)
		{
			status = joint.getScale(scale);
			status = joint.getRotation(rotation);
			translation = joint.getTranslation(MSpace::kObject, &status);
			translationTS = joint.getTranslation(MSpace::kTransform, &status);
			//translationWS = joint.getTranslation(MSpace::kWorld, &status);

			status = joint.getRotationQuaternion(rotateQuat.x, rotateQuat.y, rotateQuat.z, rotateQuat.w, MSpace::kObject);
			status = joint.getRotationQuaternion(rotateQuatTS.x, rotateQuatTS.y, rotateQuatTS.z, rotateQuatTS.w, MSpace::kTransform);
			//status = joint.getRotationQuaternion(rotateQuatWS.x, rotateQuatWS.y, rotateQuatWS.z, rotateQuatWS.w, MSpace::kWorld);

			status = joint.getOrientation(rotateOrient);
			rotateOrientOS = joint.rotateOrientation(MSpace::kObject);
			rotateOrientTS = joint.rotateOrientation(MSpace::kTransform);
			rotateOrientWS = joint.rotateOrientation(MSpace::kWorld);
		}
		else
		{
			status = fn.getScale(scale);
			status = fn.getRotation(rotation);
			translation = fn.getTranslation(MSpace::kObject);
			translationTS = fn.getTranslation(MSpace::kTransform);
			translationWS = fn.getTranslation(MSpace::kWorld);

			status = fn.getRotationQuaternion(rotateQuat.x, rotateQuat.y, rotateQuat.z, rotateQuat.w, MSpace::kObject);
			status = fn.getRotationQuaternion(rotateQuatTS.x, rotateQuatTS.y, rotateQuatTS.z, rotateQuatTS.w, MSpace::kTransform);
			//status = fn.getRotationQuaternion(rotateQuatWS.x, rotateQuatWS.y, rotateQuatWS.z, rotateQuatWS.w, MSpace::kWorld);
			rotateOrient = fn.rotateOrientation(MSpace::kObject);
			rotateOrientTS = fn.rotateOrientation(MSpace::kTransform);
			rotateOrientWS = fn.rotateOrientation(MSpace::kWorld);
		}

		//(*transformNodeAddress) = g_Out.allocate_node(::rapidxml::node_element, "Transform");
		auto transformationNode = g_Out.allocate_node(::rapidxml::node_element, "Transform");
		//auto transformationNode = g_Out.allocate_node(::rapidxml::node_element, "Transform-TestCase");
		(*transformNodeAddress) = transformationNode;

		auto transformationNodeName = g_Out.allocate_string(name.asChar());
		transformationNode->append_attribute(g_Out.allocate_attribute("Name", transformationNodeName));
		if (isJoint) transformationNode->append_attribute(g_Out.allocate_attribute("IsJoint", "1"));
		hostXmlNode->append_node(transformationNode);
		AddFamily(transformationNode, dagNode);


		auto translationNode = g_Out.allocate_node(::rapidxml::node_element, "Translate");
		auto translationNodeTS = g_Out.allocate_node(::rapidxml::node_element, "Translate");
		auto translationNodeWS = g_Out.allocate_node(::rapidxml::node_element, "Translate");

		auto tx = g_Out.allocate_string(std::to_string(translation.x).c_str());
		auto ty = g_Out.allocate_string(std::to_string(translation.y).c_str());
		auto tz = g_Out.allocate_string(std::to_string(translation.z).c_str());
		translationNode->append_attribute(g_Out.allocate_attribute("Space", "Object"));
		translationNode->append_attribute(g_Out.allocate_attribute("X", tx));
		translationNode->append_attribute(g_Out.allocate_attribute("Y", ty));
		translationNode->append_attribute(g_Out.allocate_attribute("Z", tz));

		tx = g_Out.allocate_string(std::to_string(translationTS.x).c_str());
		ty = g_Out.allocate_string(std::to_string(translationTS.y).c_str());
		tz = g_Out.allocate_string(std::to_string(translationTS.z).c_str());
		translationNodeTS->append_attribute(g_Out.allocate_attribute("Space", "Transform"));
		translationNodeTS->append_attribute(g_Out.allocate_attribute("X", tx));
		translationNodeTS->append_attribute(g_Out.allocate_attribute("Y", ty));
		translationNodeTS->append_attribute(g_Out.allocate_attribute("Z", tz));

		tx = g_Out.allocate_string(std::to_string(translationWS.x).c_str());
		ty = g_Out.allocate_string(std::to_string(translationWS.y).c_str());
		tz = g_Out.allocate_string(std::to_string(translationWS.z).c_str());
		translationNodeWS->append_attribute(g_Out.allocate_attribute("Space", "World"));
		translationNodeWS->append_attribute(g_Out.allocate_attribute("X", tx));
		translationNodeWS->append_attribute(g_Out.allocate_attribute("Y", ty));
		translationNodeWS->append_attribute(g_Out.allocate_attribute("Z", tz));

		auto scaleNode = g_Out.allocate_node(::rapidxml::node_element, "Scale");
		auto sx = g_Out.allocate_string(std::to_string(scale[0]).c_str());
		auto sy = g_Out.allocate_string(std::to_string(scale[1]).c_str());
		auto sz = g_Out.allocate_string(std::to_string(scale[2]).c_str());
		scaleNode->append_attribute(g_Out.allocate_attribute("X", sx));
		scaleNode->append_attribute(g_Out.allocate_attribute("Y", sy));
		scaleNode->append_attribute(g_Out.allocate_attribute("Z", sz));

		auto rotationNode = g_Out.allocate_node(::rapidxml::node_element, "Rotate");
		auto eulerNode = g_Out.allocate_node(::rapidxml::node_element, "Euler-Angles");
		auto quatNode = g_Out.allocate_node(::rapidxml::node_element, "Quaternion");
		auto quatNodeWS = g_Out.allocate_node(::rapidxml::node_element, "Quaternion");
		auto quatNodeTS = g_Out.allocate_node(::rapidxml::node_element, "Quaternion");
		auto orientNode = g_Out.allocate_node(::rapidxml::node_element, "Orientation");
		auto orientNodeOS = g_Out.allocate_node(::rapidxml::node_element, "Orientation");
		auto orientNodeWS = g_Out.allocate_node(::rapidxml::node_element, "Orientation");
		auto orientNodeTS = g_Out.allocate_node(::rapidxml::node_element, "Orientation");

		auto rx = g_Out.allocate_string(std::to_string(rotation.x).c_str());
		auto ry = g_Out.allocate_string(std::to_string(rotation.y).c_str());
		auto rz = g_Out.allocate_string(std::to_string(rotation.z).c_str());
		auto rw = g_Out.allocate_string(std::to_string(rotation.z).c_str());
		eulerNode->append_attribute(g_Out.allocate_attribute("X", rx));
		eulerNode->append_attribute(g_Out.allocate_attribute("Y", ry));
		eulerNode->append_attribute(g_Out.allocate_attribute("Z", rz));

		rx = g_Out.allocate_string(std::to_string(rotateQuat.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateQuat.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateQuat.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateQuat.w).c_str());
		quatNode->append_attribute(g_Out.allocate_attribute("Space", "Object"));
		quatNode->append_attribute(g_Out.allocate_attribute("X", rx));
		quatNode->append_attribute(g_Out.allocate_attribute("Y", ry));
		quatNode->append_attribute(g_Out.allocate_attribute("Z", rz));
		quatNode->append_attribute(g_Out.allocate_attribute("W", rw));

		rx = g_Out.allocate_string(std::to_string(rotateQuatTS.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateQuatTS.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateQuatTS.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateQuatTS.w).c_str());
		quatNodeTS->append_attribute(g_Out.allocate_attribute("Space", "Transform"));
		quatNodeTS->append_attribute(g_Out.allocate_attribute("X", rx));
		quatNodeTS->append_attribute(g_Out.allocate_attribute("Y", ry));
		quatNodeTS->append_attribute(g_Out.allocate_attribute("Z", rz));
		quatNodeTS->append_attribute(g_Out.allocate_attribute("W", rw));

		rx = g_Out.allocate_string(std::to_string(rotateQuatWS.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateQuatWS.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateQuatWS.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateQuatWS.w).c_str());
		quatNodeWS->append_attribute(g_Out.allocate_attribute("Space", "World"));
		quatNodeWS->append_attribute(g_Out.allocate_attribute("X", rx));
		quatNodeWS->append_attribute(g_Out.allocate_attribute("Y", ry));
		quatNodeWS->append_attribute(g_Out.allocate_attribute("Z", rz));
		quatNodeWS->append_attribute(g_Out.allocate_attribute("W", rw));

		if (isJoint)
		{
			rx = g_Out.allocate_string(std::to_string(rotateOrient.x).c_str());
			ry = g_Out.allocate_string(std::to_string(rotateOrient.y).c_str());
			rz = g_Out.allocate_string(std::to_string(rotateOrient.z).c_str());
			rw = g_Out.allocate_string(std::to_string(rotateOrient.w).c_str());
			orientNode->append_attribute(g_Out.allocate_attribute("Space", "*Object"));
			orientNode->append_attribute(g_Out.allocate_attribute("X", rx));
			orientNode->append_attribute(g_Out.allocate_attribute("Y", ry));
			orientNode->append_attribute(g_Out.allocate_attribute("Z", rz));
			orientNode->append_attribute(g_Out.allocate_attribute("W", rw));
		}

		rx = g_Out.allocate_string(std::to_string(rotateOrientOS.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateOrientOS.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateOrientOS.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateOrientOS.w).c_str());
		orientNodeOS->append_attribute(g_Out.allocate_attribute("Space", "Object"));
		orientNodeOS->append_attribute(g_Out.allocate_attribute("X", rx));
		orientNodeOS->append_attribute(g_Out.allocate_attribute("Y", ry));
		orientNodeOS->append_attribute(g_Out.allocate_attribute("Z", rz));
		orientNodeOS->append_attribute(g_Out.allocate_attribute("W", rw));

		rx = g_Out.allocate_string(std::to_string(rotateOrientTS.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateOrientTS.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateOrientTS.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateOrientTS.w).c_str());
		orientNodeTS->append_attribute(g_Out.allocate_attribute("Space", "Transform"));
		orientNodeTS->append_attribute(g_Out.allocate_attribute("X", rx));
		orientNodeTS->append_attribute(g_Out.allocate_attribute("Y", ry));
		orientNodeTS->append_attribute(g_Out.allocate_attribute("Z", rz));
		orientNodeTS->append_attribute(g_Out.allocate_attribute("W", rw));

		rx = g_Out.allocate_string(std::to_string(rotateOrientWS.x).c_str());
		ry = g_Out.allocate_string(std::to_string(rotateOrientWS.y).c_str());
		rz = g_Out.allocate_string(std::to_string(rotateOrientWS.z).c_str());
		rw = g_Out.allocate_string(std::to_string(rotateOrientWS.w).c_str());
		orientNodeWS->append_attribute(g_Out.allocate_attribute("Space", "World"));
		orientNodeWS->append_attribute(g_Out.allocate_attribute("X", rx));
		orientNodeWS->append_attribute(g_Out.allocate_attribute("Y", ry));
		orientNodeWS->append_attribute(g_Out.allocate_attribute("Z", rz));
		orientNodeWS->append_attribute(g_Out.allocate_attribute("W", rw));

		rotationNode->append_node(eulerNode);
		rotationNode->append_node(quatNode);
		rotationNode->append_node(quatNodeTS);
		rotationNode->append_node(quatNodeWS);
		if (isJoint)rotationNode->append_node(orientNode);
		rotationNode->append_node(orientNodeOS);
		rotationNode->append_node(orientNodeTS);
		rotationNode->append_node(orientNodeWS);

		transformationNode->append_node(translationNode);
		transformationNode->append_node(translationNodeTS);
		transformationNode->append_node(translationNodeWS);
		transformationNode->append_node(rotationNode);
		transformationNode->append_node(scaleNode);
	}

	void ExportTransformations2()
	{
		auto transformationsNode = g_Out.allocate_node(::rapidxml::node_element, "Transforms");
		g_DataNode->append_node(transformationsNode);

		std::cout << "Exporting > Transforms ..." << std::endl;

		MStatus status;
		UINT32 transformationNodeCount = 0;
		MItDag transformIterator(MItDag::kDepthFirst, MFn::kTransform, &status);

		for (; !transformIterator.isDone(); transformIterator.next())
		{
			MDagPath transformPath;
			status = transformIterator.getPath(transformPath);

			MFnTransform transformFn(transformPath, &status);

			if (status == MS::kSuccess)
			{
				UINT32 curvesFound = 0;
				transformationNodeCount++;

				decltype(transformationsNode) transformationNode = g_Out.allocate_node(::rapidxml::node_element, "Transform*");
				ExportTransformation2(transformationsNode, transformFn.object(), &transformationNode);

				if (g_ExportAnimationCurves && 
					MAnimUtil::isAnimated(transformPath, FALSE, &status) && 
					status == MS::kSuccess)
				{
					ExportAnimationCurves2(
						transformationNode, transformPath, 
						curvesFound
						);
				}

				auto _curvesFound = g_Out.allocate_string(std::to_string(curvesFound).c_str());
				transformationNode->append_attribute(g_Out.allocate_attribute("Curve-Count", _curvesFound));
			}
		}

		auto _transformsFound = g_Out.allocate_string(std::to_string(transformationNodeCount).c_str());
		std::cout << _transformsFound << " Transforms exported" << std::endl << std::endl;

		transformationsNode->append_attribute(g_Out.allocate_attribute("Count", _transformsFound));
	}

	void Run()
	{
		g_ExportShapes = g_Lua.get<::INT32>("exporter_config.export_shapes");
		g_ExportSkinClusters = g_Lua.get<::INT32>("exporter_config.export_skin_clusters");
		g_ExportJointClusters = g_Lua.get<::INT32>("exporter_config.export_joint_clusters");
		g_ExportTransformations = g_Lua.get<::INT32>("exporter_config.export_transformations");
		g_ExportAnimationCurves = g_Lua.get<::INT32>("exporter_config.export_animation_curves");

		std::cout << "Exporting:" << std::endl;
		std::cout << "> Shapes=" << (g_ExportShapes ? "Yes" : "No") << std::endl;
		std::cout << "> SkinClusters=" << (g_ExportSkinClusters ? "Yes" : "No") << std::endl;
		std::cout << "> Transformations=" << (g_ExportTransformations ? "Yes" : "No") << std::endl;
		std::cout << "> AnimationCurves=" << (g_ExportAnimationCurves ? "Yes" : "No") << std::endl;
		std::cout << std::endl;

		Start();

		if (g_ExportShapes) ExportShapes();
		if (g_ExportTransformations) ExportTransformations2();
		if (g_ExportSkinClusters) ExportSkinClusters();

		Finalize();
	}

	int LuaCall_Run(lua_State *)
	{
		Run(); return 0;
	}
}

namespace Flow
{
	void StartExporting()
	{
		rapidxml::xml_node<> *decl = g_Out.allocate_node(rapidxml::node_declaration);
		decl->append_attribute(g_Out.allocate_attribute("version", "1.0"));
		decl->append_attribute(g_Out.allocate_attribute("encoding", "utf-8"));
		g_Out.append_node(decl);
	}
	
	void FinishExporting()
	{
		std::string content;
		rapidxml::print(std::back_inserter(content), g_Out);
		std::ofstream outputStream(g_OutPath);
		outputStream << content;
		outputStream.close();
		g_Out.clear();
	}

	void OpenLua()
	{
		lua_register(g_Lua, "run", &::Exporter::LuaCall_Run);
		luaL_dofile(g_Lua, g_ConfigPath.c_str());
	}

	void ProcessInput()
	{
		if (g_Lua)
		{
			::INT32 error = LUA_ERRSYNTAX; // LUA_OK;
			luaL_dostring(g_Lua, "run() exit()");

			while (!error)
			{
				std::string command;
				std::getline(std::cin, command);

				if (command == "exit()")
				{
					std::cout << "Exiting...\n";
					break;
				}

				error = luaL_loadstring(g_Lua, command.c_str());

				if (error != LUA_OK)
				{
					if (error == LUA_ERRSYNTAX) std::cerr << "SYNTAX ERROR: " << lua_tostring(g_Lua, -1) << std::endl;
					else std::cerr << "UNEXPECTED (LOAD): " << lua_tostring(g_Lua, -1) << std::endl;
					lua_pop(g_Lua, 1);
					error = 0;
					continue;
				}

				error = lua_pcall(g_Lua, 0, LUA_MULTRET, 0);

				if (error != LUA_OK)
				{
					if (error == LUA_ERRRUN) std::cerr << "RUN ERROR: " << lua_tostring(g_Lua, -1) << std::endl;
					else std::cerr << "UNEXPECTED (RUN): " << lua_tostring(g_Lua, -1) << std::endl;
					lua_pop(g_Lua, 1);
					error = 0;
					continue;
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	g_App = argv[0];

#define __Test_shapes 0 //1
#define __Test_curves 0 //1
#define __Test_clusters 1

#if __Test_shapes
	if (argc >= 2) g_InPath = argv[1]; else g_InPath = "D:/Development/Maya/Scenes/scene1.ma";
	if (argc >= 3) g_OutPath = argv[2]; else g_OutPath = "D:/Development/Maya/Scenes/scene1.ma.xml";
#endif

#if __Test_curves
	if (argc >= 2) g_InPath = argv[1]; else g_InPath = "D:/Development/Maya/Scenes/joints-anim-1.ma";
	if (argc >= 3) g_OutPath = argv[2]; else g_OutPath = "D:/Development/Maya/Scenes/joints-anim-1.ma.xml";
#endif

#if __Test_clusters
	if (argc >= 2) g_InPath = argv[1]; else g_InPath = "D:/Development/Maya/Scenes/shape-6-posed.ma";
	if (argc >= 3) g_OutPath = argv[2]; else g_OutPath = "D:/Development/Maya/Scenes/shape-6-posed.ma.xml";
#endif

	if (argc >= 4) g_ConfigPath = argv[3]; else g_ConfigPath = "import-all.lua";


	std::cout << "Input  > " << g_InPath << std::endl;
	std::cout << "Output > " << g_OutPath << std::endl;
	std::cout << "Config > " << g_ConfigPath << std::endl;
	std::cout << std::endl;

	Flow::OpenLua();
	Flow::StartExporting();
	Exporter::Run();
	//Flow::ProcessInput();
	Flow::FinishExporting();

	std::cout << "Press <Enter> to exit." << std::endl; getchar();
	return EXIT_SUCCESS;
}
