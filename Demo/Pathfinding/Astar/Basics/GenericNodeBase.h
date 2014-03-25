#include <Windows.h>
#include <debugapi.h>

#ifndef __REIGN_GENERICNODEBASE_H__
#define __REIGN_GENERICNODEBASE_H__

namespace Demo
{
	namespace Standard
	{
		typedef unsigned __int32 uint32;
		typedef __int32 int32;
		typedef float float32;
		typedef unsigned char boolean;
		typedef char _Short_msg[256];
		typedef char _Intme_msg[512];
		typedef char _Large_msg[1024];

		template <typename _Ty> inline void DestroyPointer(_Ty p) { if (p) { delete p; p = nullptr; } }
		template <typename _Ty> inline void DestroyPointerArray(_Ty p) { if (p) { delete [] p; p = nullptr; } }

	}

	using namespace Standard;
}

namespace Demo
{
	namespace Standard
	{
		struct uint32x2;
		struct int32x2;

		struct uint32x2
		{
			union { struct { uint32 x, y; }; struct { uint32 i, j; }; struct { uint32 c, r; }; uint32 v[2]; };
			uint32 operator [](uint32 i) { return v[i]; }
			uint32x2(uint32 x, uint32 y) : x(x), y(y) { }
			uint32x2(uint32 v) : x(v), y(v) { }
			uint32x2() : uint32x2(0) { }
			uint32x2 operator +(uint32x2 const &b) { return uint32x2(x + b.x, y + b.y); }
			uint32x2 operator -(uint32x2 const &b) { return uint32x2(x - b.x, y - b.y); }
		};

		struct int32x2
		{
			union { struct { int32 x, y; }; struct { int32 i, j; }; struct { int32 c, r; }; int32 v[2]; };
			int32x2 operator [](int32 i) { return v[i]; }
			int32x2(int32 x, int32 y) : x(x), y(y) { }
			int32x2(int32 v) : x(v), y(v) { }
			int32x2() : int32x2(0) { }
			int32x2 operator +(int32x2 const &b) { return int32x2(x + b.x, y + b.y); }
			int32x2 operator -(int32x2 const &b) { return int32x2(x - b.x, y - b.y); }

		};

		template <typename _Ty> static int32x2 ConvertToInt32x2(_Ty const &v)
		{
			return int32x2((int32) v.x, (int32) v.y);
		}

		template <typename _Ty> static uint32x2 ConvertToUInt32x2(_Ty const &v)
		{
			return uint32x2((uint32) v.x, (uint32) v.y);
		}

	}
}

namespace Demo
{
	template <typename TNode, uint32 _AdjCount> class NodeBase; // Generic node base.
	template <typename TNode, uint32 _AdjCount> class NodeBase // Generic node base.
	{
	public:
		TNode *Adjacents[_AdjCount];
		NodeBase() { for (auto &adj : Adjacents) adj = nullptr; /*OutputDebugStringA("Node created.\n");*/ }
		~NodeBase() { /*OutputDebugStringA("Node destroyed.\n");*/ }

	public:
		inline TNode *GetNodeAt(int32 nodeIndex) { if (nodeIndex >= 0 && nodeIndex < _AdjCount) return Adjacents[nodeIndex]; else return nullptr; }
		inline TNode *GetNodeAt(uint32 nodeIndex) { if (nodeIndex < _AdjCount) return Adjacents[nodeIndex]; else return nullptr; }
		inline void SetNodeAt(int32 nodeIndex, TNode *node) { if (nodeIndex >= 0 && nodeIndex < _AdjCount) Adjacents[nodeIndex] = node; }
		inline void SetNodeAt(uint32 nodeIndex, TNode *node) { if (nodeIndex < _AdjCount) Adjacents[nodeIndex] = node; }
		inline bool HasNodeAt(uint32 nodeIndex) { if (nodeIndex < _AdjCount) return Adjacents[nodeIndex] != nullptr; }
		inline static uint32 GetNodesCount() { return _AdjCount; }
	};
}

#endif // !__REIGN_GENERICNODEBASE_H__

