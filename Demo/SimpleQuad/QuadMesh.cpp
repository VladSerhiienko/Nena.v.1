#include "QuadMesh.h"

Framework::Application::QuadMesh::QuadMesh() : 
	FrontCounterClockwise(FALSE),
	m_memoryHandle(NULL),
	Vertices(nullptr),
	Indices(nullptr),
	LastError(S_OK)
{
	using namespace Nena::Graphics::Resources;

	IndexBufferFormat = DXGI_FORMAT_R16_UINT;
	Eye = Nena::Vector3(0, 0, -1);
	Focus = Nena::Vector3(0, 0, 10);
	FOV = Nena::XM_PIDIV2; // or / 4
	AR = 800.0f / 600.0f; // w / h
	NP = 0.01f;
	FP = 100.0f;

	AllocateMemory();

}

Framework::Application::QuadMesh::~QuadMesh()
{
	DeallocateMemory();
}

void Framework::Application::QuadMesh::DeallocateMemory()
{
	// frees all the allocated memory
	//if (m_memoryHandle) ::free(m_memoryHandle);
	using namespace Nena::Graphics::Resources;
	FreeSequence(Vertices);
	FreeSequence(Indices);
}

void Framework::Application::QuadMesh::GenerateDefaultTransformation()
{
	Nena::Vector3 up(0, 1, 0);

	XformBytes = sizeof Transformation;
	Xform.World = Nena::Matrix::CreateTranslation(0, 0, 1); 
	Xform.View = Nena::Matrix::CreateLookAt(Eye, Focus, up);
	Xform.Proj = Nena::Matrix::CreatePerspectiveFieldOfView(FOV, AR, NP, FP);

	Xform.World = Xform.World.Transpose();
	Xform.View = Xform.View.Transpose();
	Xform.Proj = Xform.Proj.Transpose();
}

void Framework::Application::QuadMesh::AllocateMemory()
{
	// ensure i wont allocate memory twice in case i will change something
	// its not required to be as fast as possible so let it be safe
	DeallocateMemory();

	VerticesCount = 4; // 4 different vertices
	IndicesCount = 3 * 2; // 3 vertices for 2 triangles

	VerticesStride = sizeof Vertex;
	IndicesStride = sizeof UINT16;

	// calculate memory requirements
	VerticesBytes = VerticesCount * VerticesStride;
	IndicesBytes = IndicesCount * IndicesStride;
	UINT16 memoryRequired = VerticesBytes + IndicesBytes;

	//m_memoryHandle = ::malloc(memoryRequired);
	//if (!m_memoryHandle) // check if allocation succeeded
	//{ 
	//	LastError = E_OUTOFMEMORY; return; // crap
	//}

	// zero new allocated memory
	//m_memoryHandle = ::memset(m_memoryHandle, 0, memoryRequired);
	::memset(VSIEDesc, 0, sizeof VSIEDesc);

	// assign to variables
	Vertices = new Vertex[VerticesCount]; // (Vertex *) (m_memoryHandle);
	Indices = new UINT16[IndicesCount]; // (UINT16 *) ((BYTE *) m_memoryHandle + (VerticesBytes - 1));
}

void Framework::Application::QuadMesh::GenerateVertices()
{
	using namespace Nena::Graphics::Resources;

	VSIEDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	VSIEDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	VSIEDescCount = 2;

	VerticesOffset = 0;
	IndicesOffset = 0;

	Topology = PrimitiveTopology::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 2-unit square
	Vertices[0].Position = Nena::Vector4(-1, +1, 0, 1);
	Vertices[1].Position = Nena::Vector4(+1, +1, 0, 1);
	Vertices[2].Position = Nena::Vector4(+1, -1, 0, 1);
	Vertices[3].Position = Nena::Vector4(-1, -1, 0, 1);

	auto getRand = []() { return (float) rand() / (float) RAND_MAX; };

	// gray
	Vertices[0].Color = Nena::Vector4(getRand(), getRand(), getRand(), 1);
	Vertices[1].Color = Nena::Vector4(getRand(), getRand(), getRand(), 1);
	Vertices[2].Color = Nena::Vector4(getRand(), getRand(), getRand(), 1);
	Vertices[3].Color = Nena::Vector4(getRand(), getRand(), getRand(), 1);

	// clockwise
	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 2;
	Indices[4] = 3;
	Indices[5] = 0;
}