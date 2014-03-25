#define _USE_MATH_DEFINES

#include "GenericNodeBase.h"

#include <math.h>
#include <iostream>
#include <Windows.h>
#include <debugapi.h>

#ifndef __REIGN_OCTANODE_H__
#define __REIGN_OCTANODE_H__

namespace Demo
{
	template <typename TNodeInfo> class OctaNode; // Node, that has 8 adjacent nodes
	template <typename TNodeInfo> class OctaNode : public NodeBase<OctaNode<TNodeInfo>, 8u> // Node, that has 8 adjacent nodes
	{
		typedef NodeBase<OctaNode<TNodeInfo>, 8u> OctaNodeBase; // Basic node, that has 8 adjacent nodes

	public:
		OctaNode() : OctaNodeBase() { }

	public:
		TNodeInfo Info;

	public:
		inline OctaNode *GetRightNode() { return OctaNodeBase::GetNodeAt(0u); } // Get right node
		inline OctaNode *GetUpperNode() { return OctaNodeBase::GetNodeAt(2u); } // Get upper node
		inline OctaNode *GetLeftNode() { return OctaNodeBase::GetNodeAt(4u); } // Get left node
		inline OctaNode *GetLowerNode() { return OctaNodeBase::GetNodeAt(6u); } // Get lower node
		inline OctaNode *GetUpperRightNode() { return OctaNodeBase::GetNodeAt(1u); } // Get upper-right node
		inline OctaNode *GetUpperLeftNode() { return OctaNodeBase::GetNodeAt(3u); } // Get upper-left node
		inline OctaNode *GetLowerLeftNode() { return OctaNodeBase::GetNodeAt(5u); } // Get lower-left node
		inline OctaNode *GetLowerRightNode() { return OctaNodeBase::GetNodeAt(7u); } // Get lower-right node

		inline void SetRightNode(OctaNode *node) { OctaNodeBase::SetNodeAt(0u, node); } // Set right node
		inline void SetUpperNode(OctaNode *node) { OctaNodeBase::SetNodeAt(2u, node); } // Set upper node
		inline void SetLeftNode(OctaNode *node) { OctaNodeBase::SetNodeAt(4u, node); } // Set left node
		inline void SetLowerNode(OctaNode *node) { OctaNodeBase::SetNodeAt(6u, node); } // Set lower node
		inline void SetUpperRightNode(OctaNode *node) { OctaNodeBase::SetNodeAt(1u, node); } // Set upper-right node
		inline void SetUpperLeftNode(OctaNode *node) { OctaNodeBase::SetNodeAt(3u, node); } // Set upper-left node
		inline void SetLowerLeftNode(OctaNode *node) { OctaNodeBase::SetNodeAt(5u, node); } // Set lower-left node
		inline void SetLowerRightNode(OctaNode *node) { OctaNodeBase::SetNodeAt(7u, node); } // Set lower-right node

		inline int32x2 GetRightNodeDelta() { return LookupDelta(0); } // Get right node
		inline int32x2 GetUpperNodeDelta() { return LookupDelta(2); } // Get upper node
		inline int32x2 GetLeftNodeDelta() { return LookupDelta(4); } // Get left node
		inline int32x2 GetLowerNodeDelta() { return LookupDelta(6); } // Get lower node
		inline int32x2 GetUpperRightNodeDelta() { return LookupDelta(1); } // Get upper-right node
		inline int32x2 GetUpperLeftNodeDelta() { return LookupDelta(3); } // Get upper-left node
		inline int32x2 GetLowerLeftNodeDelta() { return LookupDelta(5); } // Get lower-left node
		inline int32x2 GetLowerRightNodeDelta() { return LookupDelta(7); } // Get lower-right node
		inline int32x2 LookupDelta(uint32 node)
		{
			static const int32x2 deltas [] =
			{
				int32x2(1, 0), // 0
				int32x2(1, 1), // 1
				int32x2(0, 1), // 2
				int32x2(-1, 1), // 3
				int32x2(-1, 0), // 4
				int32x2(-1, -1), // 5
				int32x2(0, -1), // 6
				int32x2(1, -1), // 7
			};

			return deltas[node];
		}

	public:

		// OctaNode (by default), or derived from OctaNode class.
		template <typename TExtendedOctaNode = OctaNode>
		// Returns an array of pointers to nodes, that are connected in a grid.
		static TExtendedOctaNode **CreateNodeGrid(int32 r, int32 c)
		{
			/// DEBUG >>>
			//static _Short_msg s_msg = { 0 };
			/// DEBUG <<<

			TExtendedOctaNode **nodes = new TExtendedOctaNode*[r * c];
			for (auto i = 0; i < c; i++) for (auto j = 0; j < r; j++)
				nodes[j * c + i] = new TExtendedOctaNode();

			auto nodeIndex = 0, neighbourNodeIndex = 0;
			TExtendedOctaNode *node = nullptr, *neighbourNode = nullptr;

			for (auto i = 0; i < c; i++) // column
				for (auto j = 0; j < r; j++) // row
				{
					nodeIndex = j * c + i;
					node = nodes[nodeIndex];

					for (auto dx = -1; dx <= 1; dx++)
					{
						auto x = i + dx; // column
						if (x >= 0 && x < c) for (auto dy = -1; dy <= 1; dy++)
						{
							auto y = j + dy; // row
							if (y >= 0 && y < r)
							{
								auto l = sqrt((float32) (dx * dx + dy * dy)); if (l != 0.0f)
								{
									neighbourNodeIndex = y * r + x;
									neighbourNode = nodes[neighbourNodeIndex];

									auto nodeLink = (uint32) (acosf((float32) dx / l) * 180.0f / M_PI / 45.0f);
									if (dy == -1) nodeLink = 8 - nodeLink;

									auto neighbourLink = (nodeLink + 4) % 8;

									node->SetNodeAt(nodeLink, neighbourNode);
									neighbourNode->SetNodeAt(neighbourLink, node);

									/// DEBUG >>>
									//sprintf_s(s_msg, "/ connect node (%d %d) to node (%d %d) with (%d =>/<= %d) /\n",
									//	i, j, x, y, nodeLink, neighbourLink
									//	);

									//OutputDebugStringA(s_msg);
									/// DEBUG <<<

								}
							}
						}
					}
				}

				return nodes;
		}

		// OctaNode (by default), or derived from OctaNode class.
		template <typename TExtendedOctaNode = OctaNode>
		// Destroys an array of pointers of nodes (nothing recursive here).
		static void DestroyNodeGrid(TExtendedOctaNode **nodeGrid, int32 r, int32 c)
		{
			auto nodeGridSize = r * c;
			for (auto nodeIndex = 0; nodeIndex < nodeGridSize; nodeIndex++)
				DestroyPointer(nodeGrid[nodeIndex]);
			DestroyPointerArray(nodeGrid);
		}

	};


}

#endif // !__REIGN_OCTANODE_H__

