

#include <list>
#include "OctaNode.h"

#ifndef __REIGN_DYNAMICPATHFINDER_H__
#define __REIGN_DYNAMICPATHFINDER_H__

namespace Demo
{
	struct DyTerrain;
	struct DyNodeInfo;
	struct DyRouteGuide;
	struct DyRouteExplorer;

	typedef OctaNode<DyNodeInfo> DyNode;
	typedef std::list<DyNodeInfo*> DyNodeInfoList;

	struct DyNodeInfo
	{
		uint32x2 Coords;
		uint32 H; // Path cost function.
		uint32 K; // Key function.
		bool Walkable; // - state for explorer.
		bool Obstacle; // - that can be detected by explorer.
		DyNode *Reference; // Points to the storage.
		DyNodeInfo* Backpointer; // b(X) (Points to the previous node.)
		DyNodeInfo* Forthpointer; // f(X) (Points to the next node.)
		enum ETag { eOpen, eClosed, eNew } T; // Indicates nodes state.

		DyNodeInfo() : 
			Backpointer(nullptr), Forthpointer(nullptr), Reference(nullptr),
			Walkable(true), Obstacle(false),
			T(eNew), K(0), H(0)
		{
		}
	};

	struct DyTerrain
	{
		uint32x2 Size;
		DyNode **Nodes;

		DyNode *GetNodeAt(uint32x2 coords)
		{
			if (coords.x >= Size.x || coords.y >= Size.y)
				return nullptr;

			return Nodes[coords.r * Size.r + coords.c];
		}

		DyNode *GetNodeAt(int32x2 coords)
		{
			auto sz = ConvertToInt32x2(Size);
			if (coords.x >= 0 && coords.x >= sz.x ||
				coords.y >= 0 && coords.y >= sz.y)
				return nullptr;

			return Nodes[coords.r * sz.r + coords.c];
		}

		void Draw() /// DEBUG >>>
		{
			static _Short_msg s_msg = { 0 };

			int32x2 ij = 0;
			OutputDebugStringA("DyTerrain ('*' - walkable, 'x' - unwalkable) :\n");
			for (ij.j = Size.j - 1; ij.j >= 0; ij.j--)
			{
				OutputDebugStringA("/ ");

				for (ij.i = 0; ij.i < (int32) Size.i; ij.i++)
				{
					sprintf_s(s_msg, " (%d %d) %c |", ij.i, ij.j,
						GetNodeAt(ij)->Info.Walkable ? '*' : 'X'
						); OutputDebugStringA(s_msg);
				}

				OutputDebugStringA("\n");
			}
		} /// DEBUG <<<

		DyTerrain(uint32 r, uint32 c) : Size(r, c)
		{
			uint32x2 ij = 0;
			Nodes = DyNode::CreateNodeGrid<DyNode>(Size.y, Size.x);

			for (ij.i = 0; ij.i < Size.i; ij.i++) // column
				for (ij.j = 0; ij.j < Size.j; ij.j++) // row
				{
					GetNodeAt(ij)->Info.Reference = GetNodeAt(ij);
					GetNodeAt(ij)->Info.Coords = ij;
				}
		}

		~DyTerrain()
		{
			DyNode::DestroyNodeGrid(Nodes, Size.x, Size.y);
		}

	};

	struct DyRouteExplorer
	{
		typedef uint32 (ModifyCostCallback)(DyRouteGuide *, DyNodeInfo *, DyNodeInfo *);

		ModifyCostCallback *OnObstacleDetected;
		DyRouteGuide *RoutePlanner; // Path finder.
		DyNodeInfo *InitNode; // Initial node.
		DyNodeInfo *ThisNode; // X - Current node.
		DyNodeInfo *GoalNode; // G - Goal state.
		DyTerrain *Terrain; // Surface.

		DyNodeInfo *Launch()
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 };
			sprintf_s(s_msg, "/ explorer is now at (%d %d) /\n", 
				ThisNode->Coords.x, ThisNode->Coords.y
				); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			auto nextState = ThisNode;
			while (nextState != GoalNode)
			{
				if (!nextState->Obstacle)
				{
					ThisNode = nextState;
					////////////////////////////////////////////////////////////////// DEBUG >>>
					sprintf_s(s_msg, "/ explorer makes a step to state at (%d %d) /\n",
						ThisNode->Coords.x, ThisNode->Coords.y
						); OutputDebugStringA(s_msg);
					////////////////////////////////////////////////////////////////// DEBUG <<<
				}
				else
				{
					////////////////////////////////////////////////////////////////// DEBUG >>>
					sprintf_s(s_msg, "/ explorer detectes obstacle state at (%d %d) /\n",
						nextState->Coords.x, nextState->Coords.y
						); OutputDebugStringA(s_msg);
					////////////////////////////////////////////////////////////////// DEBUG <<<

					auto Kmin = OnObstacleDetected(RoutePlanner, ThisNode, nextState);
					break;
				}

				nextState = nextState->Backpointer;

				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ explorer examines the state at (%d %d) /\n",
					nextState->Coords.x, nextState->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<
			}

			if (nextState == GoalNode)
			{
				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ explorer reaches the goal state at (%d %d) /\n",
					GoalNode->Coords.x, GoalNode->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<

				ThisNode = GoalNode;
			}

			return nextState;
		}
	};

	struct DyRouteGuide
	{
		friend DyRouteExplorer;

		DyRouteExplorer *Explorer; // Robot
		uint32 ArcCostDirect;
		uint32 ArcCostDiagon;
		uint32 ArcCostObstacle;
		uint32 EstDistDirect;
		bool AvoidCorners;

		DyRouteGuide() : m_time(0)
		{
			ArcCostDiagon = 14;
			ArcCostDirect = 10;
			EstDistDirect = 10;
			AvoidCorners = true;
		}

		void Launch()
		{
			ArcCostObstacle = Explorer->Terrain->Size.x * Explorer->Terrain->Size.y * ArcCostDiagon * ArcCostDiagon;
			Explorer->ThisNode = Explorer->InitNode;
			Explorer->OnObstacleDetected = ModifyCost;
			Explorer->RoutePlanner = this;
			Insert(Explorer->GoalNode, 0);

			auto Kmin = ProcessState();
			while (Explorer->InitNode->T != DyNodeInfo::eClosed && Kmin != UINT_MAX)
			{
				Kmin = ProcessState();
				PrintState();
				m_time++;
			}
			TracePath();

			auto explorerNode = Explorer->Launch(); // Y
			auto explore = explorerNode != Explorer->GoalNode;
			Kmin = ProcessState();
			while (explore && Kmin != UINT_MAX)
			{
				while (Kmin < Explorer->ThisNode->H && Kmin != UINT_MAX)
				{
					Kmin = ProcessState();
					PrintState();
					m_time++;
				}

				TracePath();
				if (Kmin != UINT_MAX)
				{
					explorerNode = Explorer->Launch();
					explore = explorerNode != Explorer->GoalNode;
					Kmin = GetKmin();
				}
				else
				{
					explore = false;
				}
			}
		}

	protected:
		DyNodeInfoList m_closedList; // CLOSED list
		DyNodeInfoList m_openList; // OPEN list
		uint32 m_time;

	protected:
		template <bool _Sort = true> void Insert(DyNodeInfo *x, uint32 Hnew) { }
		template <bool _Sort = true> void Delete(DyNodeInfo *x) { }
		static bool _Cmp_K_Desc(DyNodeInfo *a, DyNodeInfo *b) { return a->K < b->K; }
		static bool _Cmp_K_Asc(DyNodeInfo *a, DyNodeInfo *b) { return a->K > b->K; }
		inline void UpdateClosedList() { m_closedList.sort(_Cmp_K_Desc); }
		inline void UpdateOpenList() { m_openList.sort(_Cmp_K_Desc); }

	protected:

		bool IsWalkable(DyNode *node)
		{
			if (node) return node->Info.Walkable;
			else return true;
		}

		bool CanMoveToAdjacentInDirection(DyNode *adj, uint32 direction)
		{
			static const uint32x2 s_directions [] =
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

		// MODIFY-COST ( X, Y, c ) / Updates the arc cost function.
		uint32 ModifyCost(DyNodeInfo *x /*explorer state*/, DyNodeInfo *y /*crash*/)
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 }; 
			sprintf_s(s_msg, "/ MODIFY-COST from y (%d %d) to x (%d %d) /\n",
				x->Coords.x, x->Coords.y, y->Coords.x, y->Coords.y
				); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			if (y->Obstacle)
			{
				y->Walkable = false; // c(X, Y) = c
				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ y is an obstacle /\n",
					x->Coords.x, x->Coords.y, y->Coords.x, y->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<
			}

			if (y->T == DyNodeInfo::eClosed) // t(X) = CLOSED then
			{
				Insert(y, y->H); // INSERT(X, h(X))
				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ INSERT(y) /\n",
					x->Coords.x, x->Coords.y, y->Coords.x, y->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<
			}

			return GetKmin(); // return GET-KMIN()
		}

		// Returns the arc cost for traversing from state Y to state X.
		// @arg X = Explorer state 
		// @arg Y = State to travel to 
		// @arg Yi = Y`s index
		uint32 ArcCostFunction(DyNodeInfo *x, DyNodeInfo *y, bool directTraverse)
		{
			return !x->Walkable || !y->Walkable 
				? ArcCostObstacle // if then
				: directTraverse // if
				? ArcCostDirect // then
				: ArcCostDiagon; // else

		}

		// PROCESS-STATE ( ) / Computes optimal path costs.
		uint32 ProcessState()
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 };
			////////////////////////////////////////////////////////////////// DEBUG <<<

			// X with the lowest k() is removed from the OPEN list

			auto x = GetMinState(); // X = MIN-STATE()
			if (!x) return UINT_MAX; // if (X = NULL) return -1;
			auto Kold = x->K; // Kold = GET-KMIN()
			Delete(x); // DELETE (X)

			////////////////////////////////////////////////////////////////// DEBUG >>>
			sprintf_s(s_msg, "/ PROCESS-STATE x ta (%d %d) / k(x) %d h(x) %d / DELETE(x) /\n",
				x->Coords.x, x->Coords.y, x->K, x->H
				); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			uint32 yi = 0; // Adjacent index.
			DyNodeInfo * y; // Y (neighbour state)
			uint32 c; // c(X,Y) (arc cost function)

			// If X is a RAISE state, its path const may not be optimal.
			// Before X propagates const changes to its neighbours, its
			// its optimal neighbours are examined to see if h(X) can be
			// reduced.

			bool directTraverse = true;

			if (Kold < x->H)// if (Kold < h(X)) then
			{
				////////////////////////////////////////////////////////////////// DEBUG >>>
				//sprintf_s(s_msg, "/ RAISE state => Kold < h(X) /\n"
				//	); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<

				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents) 
					if (adjacentNode != nullptr /*&& AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yi)*/)
					{
						y = &adjacentNode->Info; // Y
						c = ArcCostFunction(y, x, directTraverse); // c(Y, X)

						////////////////////////////////////////////////////////////////// DEBUG >>>
						//sprintf_s(s_msg, "/ neighbour y at (%d %d) / k(y) %d h(y) %d c(y, x) %d /\n",
						//	y->Coords.x, y->Coords.y, y->K, y->H, c
						//	); OutputDebugStringA(s_msg);
						////////////////////////////////////////////////////////////////// DEBUG <<<

						if ((y->H <= Kold) && // h(Y) <= Kold and
							(x->H > (y->H + c))) // h(X) > h(Y) + c(Y,X) then
						{
							y->Forthpointer = x; // f(Y) = X
							x->Backpointer = y; // b(X) = Y
							x->H = y->H + c; // h(X) = h(Y) + c(Y,X)
							UpdateOpenList();

							////////////////////////////////////////////////////////////////// DEBUG >>>
							//sprintf_s(s_msg, "/ b(x) = y / h(x) = h(y) + c(y,x) = %d /\n",
							//	x->H ); OutputDebugStringA(s_msg);
							////////////////////////////////////////////////////////////////// DEBUG <<<
						}

						directTraverse = !directTraverse;
						yi++;
					}
					else
					{
						directTraverse = !directTraverse;
						yi++;
					}
			}

			// If X is a LOWER state its path cost is optimal 
			// since h(X) is equal to the old Kmin.

			if (Kold == x->H) // if (Kold = h(X)) then
			{
				////////////////////////////////////////////////////////////////// DEBUG >>>
				//sprintf_s(s_msg, "/ LOWER state => Kold == h(X) /\n"
				//	); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<

				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents)
					if (adjacentNode != nullptr /*&& AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yi)*/)
					{
						y = &adjacentNode->Info; // Y
						c = ArcCostFunction(x, y, directTraverse); // c(X,Y)

						////////////////////////////////////////////////////////////////// DEBUG >>>
						//sprintf_s(s_msg, "/ neighbour y at (%d %d) / k(y) %d h(y) %d c(x, y) %d /\n",
						//	y->Coords.x, y->Coords.y, y->K, y->H, c
						//	); OutputDebugStringA(s_msg);
						////////////////////////////////////////////////////////////////// DEBUG <<<

						if ((y->T == DyNodeInfo::eNew) || // t(Y) == NEW or
							((y->Backpointer == x) && (y->H != (x->H + c))) || // (b(Y) == X and h(Y) != h(X) + c(X,Y)) or 
							((y->Backpointer != x) && (y->H > (x->H + c)))) // (b(Y) != X and h(Y) > h(X) + c(X,Y)) then
						{
							x->Forthpointer = y; // f(X) = Y
							y->Backpointer = x; // b(Y) = X
							Insert(y, x->H + c); // INSERT(Y, h(X) + c(X,Y))

							////////////////////////////////////////////////////////////////// DEBUG >>>
							//sprintf_s(s_msg, "/ b(y) = x / INSERT(y, h(x) + c(x,y)) /\n"
							//	); OutputDebugStringA(s_msg);
							////////////////////////////////////////////////////////////////// DEBUG <<<
						}

						directTraverse = !directTraverse;
						yi++;
					}
					else
					{
						directTraverse = !directTraverse;
						yi++;
					}
			}

			else
			{
				// Cost changes are propagated to NEW states and 
				// immediate descendants in the same way as for 
				// LOWER states

				////////////////////////////////////////////////////////////////// DEBUG >>>
				//sprintf_s(s_msg, "/ Propagate cost changes /\n"
				//	); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<

				auto yii = 0u;

				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents)
					if (adjacentNode != nullptr/* && AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yii)*/)
					{
						y = &adjacentNode->Info; // Y
						c = ArcCostFunction(x, y, directTraverse); // c(X,Y)

						////////////////////////////////////////////////////////////////// DEBUG >>>
						//sprintf_s(s_msg, "/ neighbour y at (%d %d) / k(y) %d h(y) %d c(x, y) %d /\n",
						//	y->Coords.x, y->Coords.y, y->K, y->H, c
						//	); OutputDebugStringA(s_msg);
						////////////////////////////////////////////////////////////////// DEBUG <<<

						if ((y->T == DyNodeInfo::eNew) || // t(Y) == NEW or
							((y->Backpointer == x) && // (b(Y) == X and 
							(y->H != (x->H + c)))) // h(Y) != h(X) + c(X, Y)) then
						{
							x->Forthpointer = y; // f(X) = Y
							y->Backpointer = x; // b(Y) = X
							Insert(y, x->H + c); // INSERT(Y, h(X) + c(X, Y))

							////////////////////////////////////////////////////////////////// DEBUG >>>
							//sprintf_s(s_msg, "/ b(y) = x / INSERT(y, h(x) + c(x,y)) /\n"
							//	); OutputDebugStringA(s_msg);
							////////////////////////////////////////////////////////////////// DEBUG <<<
						}

						// If X is able to lower the path cost of state 
						// that is not an immediate descedant, X is 
						// placed back to open list for future expansion.

						else
						{
							if ((y->Backpointer != x) && // b(Y) != X and
								(y->H > (x->H + c))) // h(Y) > h(X) + c(X, Y) then
							{
								Insert(x, x->H); // INSERT(X, h(X))

								////////////////////////////////////////////////////////////////// DEBUG >>>
								//sprintf_s(s_msg, "/ INSERT(x, h(x)) /\n"
								//	); OutputDebugStringA(s_msg);
								////////////////////////////////////////////////////////////////// DEBUG <<<
							}

							// If the path cost of X ia able to be reproduced by a 
							// suboptimal neighbour, the neighbour is placed back 
							// on the OPEN list. Thus, the update is postponed until 
							// the neighbour has an optimal path cost.

							else
							{
								c = ArcCostFunction(y, x, directTraverse); // c(Y, X)
								if ((y->Backpointer != x) && // b(Y) != X and
									(x->H > (y->H + c)) &&  // h(X) > h(Y) + c(Y,X) and
									(y->T == DyNodeInfo::eClosed) && // t(Y) = CLOSED and
									(y->H > Kold)) // h(Y) > Kold then
								{
									Insert(y, y->H); // INSERT(X, h(X))

									////////////////////////////////////////////////////////////////// DEBUG >>>
									//sprintf_s(s_msg, "/ INSERT(y, h(y)) /\n"
									//	); OutputDebugStringA(s_msg);
									////////////////////////////////////////////////////////////////// DEBUG <<<
								}
							}
						}

						directTraverse = !directTraverse;
						yii++;
					}
					else
					{
						directTraverse = !directTraverse;
						yii++;
					}
			}

			return GetKmin(); // return GET-KMIN()
		}

		static uint32 ModifyCost(DyRouteGuide *guide, DyNodeInfo *x, DyNodeInfo *y)
		{
			return guide->ModifyCost(x, y);
		}

	protected:

		// GET-KMIN ( ) / Returns min { k(X) } for all X on the OPEN list or UINT_MAX if its empty.
		uint32 GetKmin() 
		{ 
			if (m_openList.empty()) return UINT_MAX;
			else return m_openList.front()->K; 
		} 

		// MIN-STATE ( ) / Returns X state with k(X) = min { k(X) } for all X on the OPEN list or nullptr if its empty.
		DyNodeInfo *GetMinState()
		{ 
			if (m_openList.empty()) return nullptr;
			else return m_openList.front();
		} 

		// INSERT ( X, Hnew ) / Places or repositions the state X in the OPEN list.
		template <> void Insert<false>(DyNodeInfo *x, uint32 Hnew)
		{
			switch (x->T)
			{
			case DyNodeInfo::eNew: /* k(X) = Hnew */ x->K = Hnew; break;
			case DyNodeInfo::eOpen: /* k(X) = min { k(X), Hnew } */ x->K = min(x->K, Hnew); break;
			case DyNodeInfo::eClosed: /* k(X) = min { h(X), Hnew } */ x->K = min(x->H, Hnew); break;
			}

			x->H = Hnew; // h(X) = Hnew
			if (x->T != DyNodeInfo::eOpen)
			{
				x->T = DyNodeInfo::eOpen; // t(X) = OPEN
				m_openList.push_back(x);
			}
		}

		// INSERT ( X, Hnew ) / Places or repositions the state X in the OPEN list and sorts it by k(X) value.
		template <> void Insert<true>(DyNodeInfo *x, uint32 Hnew)
		{
			Insert<false>(x, Hnew);
			UpdateOpenList();
		}

		// DELETE ( X ) / Deletes state X from the OPEN list.
		template <> void Delete<false>(DyNodeInfo *x) 
		{
			if (x->T != DyNodeInfo::eClosed)
			{
				x->T = DyNodeInfo::eClosed; // t(X) = CLOSED
				//m_closedList.push_back(x);
				m_openList.remove(x);
			}
		}

		// DELETE ( X ) / Deletes state X from the OPEN list and sorts it by k(X) value.
		template <> void Delete<true>(DyNodeInfo *x)
		{
			Delete<false>(x);
			//UpdateClosedList();
		}

	protected:

		void TracePath()
		{
			////////////////////////////////////////////////////////////////// DEBUG >>>
			static _Short_msg s_msg = { 0 };
			sprintf_s(s_msg, "Tracing the path: \n"); OutputDebugStringA(s_msg);
			////////////////////////////////////////////////////////////////// DEBUG <<<

			auto prevNode = Explorer->ThisNode; while (prevNode != nullptr)
			{
				////////////////////////////////////////////////////////////////// DEBUG >>>
				sprintf_s(s_msg, "/ => node (%d %d) \n",
					prevNode->Coords.x, prevNode->Coords.y
					); OutputDebugStringA(s_msg);
				////////////////////////////////////////////////////////////////// DEBUG <<<
				prevNode = prevNode->Backpointer;
			}
		}

		void PrintState() /// DEBUG >>>
		{
			static _Short_msg s_msg = { 0 };

			int32x2 ij = 0;

			sprintf_s(s_msg, "State (time=%d):\n", m_time); 
			OutputDebugStringA(s_msg);

			OutputDebugStringA("Y/X       / ");
			for (ij.i = 0; ij.i < (int32) Explorer->Terrain->Size.i; ij.i++)
			{
				sprintf_s(s_msg, "%-4d       /", ij.i);
				OutputDebugStringA(s_msg);
			}

			OutputDebugStringA("\n");

			for (ij.j = Explorer->Terrain->Size.j - 1; ij.j >= 0; ij.j--)
			{
				sprintf_s(s_msg, "%-4d      / ", ij.j);
				OutputDebugStringA(s_msg);

				for (ij.i = 0; ij.i < (int32) Explorer->Terrain->Size.i; ij.i++)
				{
					auto node = &Explorer->Terrain->GetNodeAt(ij)->Info;

					char tag = '*';

					if (!node->Walkable) tag = 'x';
					if (node->T == NodeInfo::eOpen) tag = 'o';
					if (node->T == NodeInfo::eClosed) tag = 'c';
					if (node->T == NodeInfo::eClosed && !node->Walkable) tag = 'w';

					sprintf_s(s_msg, "%-4d %-4d %c|", node->H, node->K, tag);
					OutputDebugStringA(s_msg);
				}

				OutputDebugStringA("\n");
			}
		} /// DEBUG <<<



		// PROCESS-STATE ( ) / Computes optimal path costs.
		uint32 ProcessState_Old()
		{
			// X with the lowest k() is removed from the OPEN list

			auto x = GetMinState(); // X = MIN-STATE()
			if (!x) return UINT_MAX; // if (X = NULL) return -1;
			auto Kold = x->K; // Kold = GET-KMIN()
			Delete(x); // DELETE (X)

			uint32 yi = 0; // Adjacent index.
			DyNodeInfo * y; // Y (neighbour state)
			uint32 c; // c(X,Y) (arc cost function)

			// If X is a RAISE state, its path const may not be optimal.
			// Before X propagates const changes to its neighbours, its
			// its optimal neighbours are examined to see if h(X) can be
			// reduced.

			if (Kold < x->H)// if (Kold < h(X)) then
			{
				uint32 yi = 0; // Adjacent index.

				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents)
					if (adjacentNode != nullptr && AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yi))
					{
						y = &adjacentNode->Info; // Y
						c = !adjacentNode->Info.Walkable
							? ArcCostObstacle
							: yi % 2 == 0 // c(X,Y)
							? ArcCostDirect
							: ArcCostDiagon;

						if ((y->H <= Kold) && // h(Y) <= Kold and
							(x->H > (y->H + c))) // h(X) > h(Y) + c(X,Y) then
						{
							y->Forthpointer = x; // f(Y) = X
							x->Backpointer = y; // b(X) = Y
							x->H = y->H + c; // h(X) = h(Y) + c(X,Y)
						}

						yi++;
					}
					else yi++;
			}

			// If X is a LOWER state its path cost is optimal 
			// since h(X) is equal to the old Kmin.

			if (Kold == x->H) // if (Kold = h(X)) then
			{
				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents)
					if (adjacentNode != nullptr && AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yi))
					{
						y = &adjacentNode->Info; // Y
						c = !adjacentNode->Info.Walkable
							? ArcCostObstacle
							: yi % 2 == 0 // c(X,Y)
							? ArcCostDirect
							: ArcCostDiagon;

						if ((y->T == DyNodeInfo::eNew) || // t(Y) == NEW or
							((y->Backpointer == x) && (y->H != (x->H + c))) || // (b(Y) == X and h(Y) != h(X) + c(X,Y)) or 
							((y->Backpointer != x) && (y->H > (x->H + c)))) // (b(Y) != X and h(Y) > h(X) + c(X,Y)) then
						{
							x->Forthpointer = y; // f(X) = Y
							y->Backpointer = x; // b(Y) = X
							Insert(y, x->H + c); // INSERT(Y, h(X) + c(X,Y))
						}

						yi++;
					}
					else yi++;
			}

			else
			{
				// Cost changes are propagated to NEW states and 
				// immediate descendants in the same way as for 
				// LOWER states

				// for each neighbour Y of X :
				for (auto &adjacentNode : x->Reference->Adjacents)
					if (adjacentNode != nullptr && AvoidCorners &&
						CanMoveToAdjacentInDirection(adjacentNode, yi))
					{
						y = &adjacentNode->Info; // Y
						c = !adjacentNode->Info.Walkable
							? ArcCostObstacle
							: yi % 2 == 0 // c(X,Y)
							? ArcCostDirect
							: ArcCostDiagon;

						if ((y->T == DyNodeInfo::eNew) || // t(Y) == NEW or
							((y->Backpointer == x) && // (b(Y) == X and 
							(y->H != (x->H + c)))) // h(Y) != h(X) + c(X, Y)) then
						{
							x->Forthpointer = y; // f(X) = Y
							y->Backpointer = x; // b(Y) = X
							Insert(y, x->H + c); // INSERT(Y, h(X) + c(X, Y))
						}

						// If X is able to lower the path cost of state 
						// that is not an immediate descedant, X is 
						// placed back to open list for future expansion.

						else
						{
							if ((y->Backpointer != x) && // b(Y) != X and
								(y->H > (x->H + c))) // h(Y) > h(X) + c(X, Y) then
							{
								Insert(x, x->H); // INSERT(X, h(X))
							}

							// If the path cost of X ia able to be reproduced by a 
							// suboptimal neighbour, the neighbour is placed back 
							// on the OPEN list. Thus, the update is postponed until 
							// the neighbour has an optimal path cost.

							else
							{
								if ((y->Backpointer != x) && // b(Y) != X and
									(x->H > (y->H + c)) &&  // h(X) > h(Y) + c(X, Y) and
									(y->T == DyNodeInfo::eClosed) && // t(Y) = CLOSED and
									(y->H > Kold)) // h(Y) > Kold then
								{
									Insert(y, y->H); // INSERT(X, h(X))
								}
							}
						}

						yi++;
					}
					else yi++;
			}

			return GetKmin(); // return GET-KMIN()
		}

	};

}

#endif // !__REIGN_DYNAMICPATHFINDER_H__
