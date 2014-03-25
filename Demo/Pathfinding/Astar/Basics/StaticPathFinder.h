#include "OctaNode.h"

#include <list>
#include <vector>

#ifndef __REIGN_PATHFINDER_H__
#define __REIGN_PATHFINDER_H__

namespace Demo
{
	struct NodeInfo;
	typedef OctaNode<NodeInfo> Node;
	typedef std::list<NodeInfo*> NodeInfoList;

	struct NodeInfo
	{
		uint32x2 Coords; // X-Y.
		bool Walkable; // Walkable.
		uint32 F, G, H; // Scores.
		Node *Reference; // Points to the storage.
		NodeInfo* Backpointer; // Points to the previous node.
		NodeInfo* Forthpointer; // Points to the next node.
		enum ETag { eOpen, eClosed, eNew } Tag; // Indicates nodes state.

		NodeInfo() : 
			Backpointer(nullptr), Forthpointer(nullptr), Reference(nullptr),
			Tag(eNew), F(0), G(0), H(0), Walkable(true)
		{
		}
	};

	/*typedef OctaNode<NodeInfo> OctaInfoNode;
	struct Node : public OctaInfoNode
	{
		typedef OctaNode<NodeInfo> NodeBase;
		Node() : NodeBase() { }
	};*/


	struct SaTerrain
	{
		uint32x2 Size;
		Node **Nodes;

		Node *GetNodeAt(uint32x2 coords)
		{
			if (coords.x >= Size.x || coords.y >= Size.y)
				return nullptr;

			return Nodes[coords.r * Size.r + coords.c];
		}

		Node *GetNodeAt(int32x2 coords)
		{
			auto sz = ConvertToInt32x2(Size);
			if (coords.x >= 0 && coords.x >= sz.x ||
				coords.y >= 0 && coords.y >= sz.y)
				return nullptr;

			return Nodes[coords.r * sz.r + coords.c];
		}

		SaTerrain(uint32 r, uint32 c) : Size(r, c) 
		{ 
			uint32x2 ij = 0;
			Nodes = Node::CreateNodeGrid<Node>(Size.y, Size.x);

			for (ij.i = 0; ij.i < Size.i; ij.i++) // column
				for (ij.j = 0; ij.j < Size.j; ij.j++) // row
				{
					GetNodeAt(ij)->Info.Reference = GetNodeAt(ij);
					GetNodeAt(ij)->Info.Coords = ij;
				}
		}

		~SaTerrain() 
		{
			Node::DestroyNodeGrid(Nodes, Size.x, Size.y); 
		}
		
		void Draw() /// DEBUG >>>
		{
			static _Short_msg s_msg = { 0 };

			int32x2 ij = 0;
			OutputDebugStringA("SaTerrain ('*' - walkable, 'x' - unwalkable) :\n");
			for (ij.j = Size.j - 1; ij.j >= 0; ij.j--)
			{
				OutputDebugStringA("/ ");

				for (ij.i = 0; ij.i < (int32)Size.i; ij.i++)
				{
					sprintf_s(s_msg, " (%d %d) %c |", ij.i, ij.j,
						GetNodeAt(ij)->Info.Walkable ? '*' : 'X'
						); OutputDebugStringA(s_msg);
				}

				OutputDebugStringA("\n");
			}
		} /// DEBUG <<<
		

	};

	enum class EOnAddToList { eError = 0, eAdded = 1, eAlreadyInList = 2, eAlreadyTagged = 4 };
	EOnAddToList operator | (EOnAddToList const &a, EOnAddToList const &b) { return (EOnAddToList) ((uint32) a | (uint32) b); }
	EOnAddToList operator & (EOnAddToList const &a, EOnAddToList const &b) { return (EOnAddToList) ((uint32) a & (uint32) b); }
	EOnAddToList operator |= (EOnAddToList &a, EOnAddToList const &b) { a = a | b; return a; }
	EOnAddToList operator &= (EOnAddToList &a, EOnAddToList const &b) { a = a & b; return a; }

	class SaPathFinder
	{
	public:

		SaPathFinder() : 
			m_initNodePosition(), m_goalNodePosition(), m_thisNodePosition(), 
			m_openList(), m_closedList(), m_newList(), 
			m_terrain(nullptr), m_thisNode(nullptr),
			m_time(0)
		{
			ArcCostDiagon = 14;
			ArcCostDirect = 10;
			EstDistDirect = 10;
			AvoidCorners = true;
		}

		void SetTerrain(SaTerrain *terrain)
		{
			m_terrain = terrain;
		}

		void SetInitNode(uint32x2 coords)
		{
			m_thisNodePosition = coords;
			m_thisNode = m_terrain->GetNodeAt(coords);
			m_initNodePosition = m_thisNodePosition;
			m_initNode = m_thisNode;
		}

		void SetGoalNode(uint32x2 coords)
		{
			m_goalNodePosition = coords;
			m_goalNode = m_terrain->GetNodeAt(coords);
		}

		void Launch()
		{
			AddToOpenList(&m_initNode->Info);

			bool trace = false;
			while (true)
			{
				// The best node from the open list.
				auto openNode = ProcessOpenList();
				m_thisNode = openNode;

				if (!openNode) break; // The open list is empty.
				if (openNode == m_goalNode) // Path found.
				{
					AddToClosedList(&m_goalNode->Info);
					trace = true;
					break; 
				}

				// Adds adjacents to the open list, records scores.
				ProcessAdjacents(openNode); 
				m_time++;
			}

			if (trace) TracePath();
		}

	protected:

		void TracePath()
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 };
			sprintf_s(s_msg, "Tracing the path: \n"); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			auto prevNode = &m_goalNode->Info; while (prevNode != nullptr)
			{
				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ => node (%d %d) \n", 
					prevNode->Coords.x, prevNode->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<
				prevNode = prevNode->Backpointer;
			}
		}

		// Add all the adjacents to the open list.
		void OpenAdjacents(Node *node)
		{
			for (auto &adj : node->Adjacents) if (adj != nullptr)
			{
				if (InClosedList(&adj->Info) || !adj->Info.Walkable)
				{
					auto onAdd = AddToOpenList(&adj->Info);
				}
			}
		}

		// Return the best node from the open list.
		Node *ProcessOpenList()
		{
			if (!m_openList.empty()) return m_openList.front()->Reference;
			else return nullptr;
		}

		bool IsWalkable(Node *node)
		{
			if (node) return node->Info.Walkable;
			else return true;
		}

		bool CanMoveInDirection(Node *adj, uint32 direction)
		{
			static uint32x2 s_directions[] =
			{
				uint32x2(4, 6),
				uint32x2(0, 6),
				uint32x2(0, 2),
				uint32x2(2, 4),
			}; 
			
			uint32x2 directions = 0; switch (direction)
			{
			case 1: directions = s_directions[0]; break;
			case 3: directions = s_directions[1]; break;
			case 5: directions = s_directions[2]; break;
			case 7: directions = s_directions[3]; break;
			}

			if (adj) return 
				IsWalkable(adj->GetNodeAt(directions[0])) &&
				IsWalkable(adj->GetNodeAt(directions[1]));
			else return false;
		}

		// Add provided node to the closed list, loop through all adjacents, 
		// update their scores, choose the one with the lowest score, 
		// link it with the provided node and return it.
		void ProcessAdjacents(Node *node, bool start = false)
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 };
			sprintf_s(s_msg, "Process adjacents for node (%d %d):\n", 
				node->Info.Coords.x, node->Info.Coords.y
				); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			uint32 score = 0; // f
			uint32 arccost = 0; // g
			uint32 manhattan = 0; // h
			uint32 adjIndex = 0; // Iterator.

			RemoveFromOpenList(&node->Info); // Remove current node from the open list.
			auto onAdd = AddToClosedList(&node->Info); // Add initial node to the closed list.
			int32x2 initCoords = ConvertToInt32x2(m_initNodePosition); // Copy and cast.

			for (auto &adj : node->Adjacents) if (adj != nullptr) // Loop through all the adjacents of the node
			{
				if (!adj->Info.Walkable)
				{
					////////////////////////////////////////////////////////////////// DEBUG >>>
					sprintf_s(s_msg, "/ node at (%d %d) => Obstacle /\n",
						adj->Info.Coords.x, adj->Info.Coords.y
						); OutputDebugStringA(s_msg);
					////////////////////////////////////////////////////////////////// DEBUG <<<

					adjIndex++;
					continue;
				}

				if (InClosedList(&adj->Info))
				{
					////////////////////////////////////////////////////////////////// DEBUG >>>
					sprintf_s(s_msg, "/ node at (%d %d) => Closed /\n",
						adj->Info.Coords.x, adj->Info.Coords.y
						); OutputDebugStringA(s_msg);
					////////////////////////////////////////////////////////////////// DEBUG <<<

					adjIndex++;
					continue;
				}

				if (AvoidCorners && adjIndex % 2 != 0) 
					if (!CanMoveInDirection(adj, adjIndex))
					{
						////////////////////////////////////////////////////////////////// DEBUG >>>
						sprintf_s(s_msg, "/ node at (%d %d) => Corner /\n",
							adj->Info.Coords.x, adj->Info.Coords.y
							); OutputDebugStringA(s_msg);
						////////////////////////////////////////////////////////////////// DEBUG <<<

						adjIndex++;
						continue;
					}

				manhattan = ManhattanDistance(ConvertToInt32x2(adj->Info.Coords), ConvertToInt32x2(m_goalNodePosition)); 
				arccost = adjIndex % 2 != 0 ? ArcCostDiagon : ArcCostDirect;
				arccost = arccost + node->Info.G;
				score = arccost + manhattan;

				if (adj->Info.Tag == NodeInfo::eOpen && adj->Info.G < arccost)
				{
					OutputDebugStringA("/ already in the open list and has better scores /\n");
					//adj->Info.Backpointer = node->Info.Backpointer->Backpointer;
					//adj->Info.Backpointer = node->Info.Backpointer;
				}
				else
				{
					OutputDebugStringA("/ add to open list and record scores /\n");
					onAdd = AddToOpenList(&adj->Info);
					adj->Info.Backpointer = &node->Info;
					node->Info.Forthpointer = &adj->Info;
					adj->Info.H = manhattan;
					adj->Info.G = arccost;
					adj->Info.F = score;
				}

				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ node at (%d %d) => F %d G %d H %d /\n",
					adj->Info.Coords.x, adj->Info.Coords.y, score, arccost, manhattan 
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<

				adjIndex++;
			}
			else adjIndex++;

			UpdateOpenList();
			PrintState();
		}

		uint32 ManhattanDistance(int32x2 a, int32x2 b)
		{
			int32x2 diff = b - a;
			diff.x = abs(diff.x);
			diff.y = abs(diff.y);

			return diff.x * EstDistDirect + 
				diff.y * EstDistDirect;
		}

		void PrintState() /// DEBUG >>>
		{
			static _Short_msg s_msg = { 0 };

			int32x2 ij = 0;

			sprintf_s(s_msg, "State (time=%d):\n", m_time);
			OutputDebugStringA(s_msg);

			OutputDebugStringA("Y/X  / ");
			for (ij.i = 0; ij.i < (int32) m_terrain->Size.i; ij.i++)
			{
				sprintf_s(s_msg, "%-4d  /", ij.i);
				OutputDebugStringA(s_msg);
			}

			OutputDebugStringA("\n");

			for (ij.j = m_terrain->Size.j - 1; ij.j >= 0; ij.j--)
			{
				sprintf_s(s_msg, "%-3d / ", ij.j); 
				OutputDebugStringA(s_msg);

				for (ij.i = 0; ij.i < (int32)m_terrain->Size.i; ij.i++)
				{
					auto node = &m_terrain->GetNodeAt(ij)->Info;
					
					char tag = '*';

					if (InOpenList(node)) tag = 'o';
					if (InClosedList(node)) tag = 'c';
					if (!node->Walkable) tag = 'X';
					if (node == &m_goalNode->Info) tag = 'G';
					if (node == &m_initNode->Info) tag = 'I';
					if (node == &m_thisNode->Info) tag = 'T';
					if (node == &m_thisNode->Info && m_thisNode == m_goalNode) tag = 'R';

					sprintf_s(s_msg, "%-4d %c|", node->F, tag ); 
					OutputDebugStringA(s_msg);
				}

				OutputDebugStringA("\n");
			}
		} /// DEBUG <<<

	protected:

		// Indicates, whether the provided list contains a desired node. 
		// Returns a node index, if found. Otherwise, returns UINT_MAX (=0xffffffff).
		uint32 ContainsNode(NodeInfoList *list, NodeInfo *node)
		{
			uint32 index = 0u; auto first = list->begin(), last = list->end();
			for (auto i = first; i != last; i++, index++) if (*i == node) return index;
			return UINT_MAX;
		}

		bool InClosedList(NodeInfo *node) { return ContainsNode(&m_closedList, node) != UINT_MAX; }
		bool InOpenList(NodeInfo *node) { return ContainsNode(&m_openList, node) != UINT_MAX; } 

	public:
		uint32 ArcCostDirect;
		uint32 ArcCostDiagon;
		uint32 EstDistDirect;
		bool AvoidCorners;

	protected:
		static bool ComparatorLess(NodeInfo *a, NodeInfo *b) { return a->F < b->F; }
		void UpdateOpenList() { return m_openList.sort(ComparatorLess); }

		EOnAddToList AddToList(NodeInfoList *list, NodeInfo *node, NodeInfo::ETag tag)
		{
			EOnAddToList result = EOnAddToList::eError;
			if (node->Tag == tag) result |= EOnAddToList::eAlreadyTagged;
			if (ContainsNode(&m_closedList, node) != UINT_MAX)
			{
				result |= EOnAddToList::eAlreadyInList;
				return result;
			}
			else
			{
				node->Tag = tag;
				result |= EOnAddToList::eAdded;
				list->push_back(node);
				return result;
			}
		}

		EOnAddToList AddToClosedList(NodeInfo *node) { return AddToList(&m_closedList, node, NodeInfo::eClosed); }
		EOnAddToList AddToOpenList(NodeInfo *node) { return AddToList(&m_openList, node, NodeInfo::eOpen); }
		EOnAddToList AddToNewList(NodeInfo *node) { return AddToList(&m_newList, node, NodeInfo::eNew); }

		void RemoveFromList(NodeInfoList *list, NodeInfo *node) { return list->remove(node); }

		void RemoveFromClosedList(NodeInfo *node) { return RemoveFromList(&m_closedList, node); }
		void RemoveFromOpenList(NodeInfo *node) { return RemoveFromList(&m_openList, node); }
		void RemoveFromNewList(NodeInfo *node) { return RemoveFromList(&m_newList, node); }

	protected:
		uint32x2 m_initNodePosition;
		uint32x2 m_goalNodePosition;
		uint32x2 m_thisNodePosition;
		NodeInfoList m_closedList;
		NodeInfoList m_openList;
		NodeInfoList m_newList;
		SaTerrain *m_terrain;
		Node *m_thisNode;
		Node *m_initNode;
		Node *m_goalNode;
		uint32 m_time;

	};

}

#endif // !__REIGN_PATHFINDER_H__
