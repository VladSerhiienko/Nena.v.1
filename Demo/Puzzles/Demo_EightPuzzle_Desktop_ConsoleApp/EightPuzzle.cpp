#include "Pch.h"
#include "EightPuzzle.h"

INT Demo::EightPuzzle::Solver::Rules::Move( _In_ INT iTile, _In_ INT iDirection )
{
	static const INT s_Rules[9][4] = 
	{
		{ 1, 3,			s_iIllegalMove, s_iIllegalMove },
		{ 0, 2, 4,		s_iIllegalMove },
		{ 1, 5,			s_iIllegalMove, s_iIllegalMove },
		{ 0, 4, 6,		s_iIllegalMove },
		{ 1, 3, 5, 7 },
		{ 2, 4, 8,		s_iIllegalMove },
		{ 3, 7,			s_iIllegalMove, s_iIllegalMove },
		{ 4, 6, 8,		s_iIllegalMove },
		{ 5, 7,			s_iIllegalMove, s_iIllegalMove },
	};

	return s_Rules[iTile][iDirection];
}

Demo::EightPuzzle::Solver::Solver( _In_ State piState, _In_ IHeuristic *pEval ) : 
	pRoot(nullptr)
{
	CopyState(piState, ix9StartState);
	pHeuristicEvalulator = pEval ? pEval : new Manhattan;
	vnNodeFringe.clear();
	bSolved = FALSE;
	nTotalSteps = 0;
}

void Demo::EightPuzzle::Solver::Dealloc( _In_ Node *pNode )
{
	if (!pNode) if (!pRoot) return; else pNode = pRoot;
	if (pNode->ppChildren[0]) Dealloc(pNode->ppChildren[0]);
	if (pNode->ppChildren[1]) Dealloc(pNode->ppChildren[1]);
	if (pNode->ppChildren[2]) Dealloc(pNode->ppChildren[2]);
	if (pNode->ppChildren[3]) Dealloc(pNode->ppChildren[3]);

	if (pNode)
	{
		delete pNode;
		pNode = nullptr;
	}
}

Demo::EightPuzzle::Solver::~Solver()
{
	Log.Grab( "Cleanup\n" );

	if (pHeuristicEvalulator)
	{
		delete pHeuristicEvalulator;
		pHeuristicEvalulator = nullptr;
	}

	vnNodeFringe.clear();
	Dealloc();
	Log.Grab();
}

INT Demo::EightPuzzle::Solver::ResolveDepth( _In_ Node *pNode, _In_opt_ INT iDepth )
{
	if (pNode->pParent) return ResolveDepth(pNode->pParent, ++iDepth);
	else return iDepth;
}

void Demo::EightPuzzle::Solver::ToFringe( Node *pNode )
{
	NodeInterator pIterator = vnNodeFringe.begin();
	while (pIterator != vnNodeFringe.end())
		if ( pNode->iScore <= (*pIterator)->iScore) break;
		else pIterator++;
	vnNodeFringe.insert(pIterator, pNode);
}

Demo::EightPuzzle::Solver::Node * Demo::EightPuzzle::Solver::AllocNewNode()
{
	auto pNode = new Node;

	if (pNode) ZeroMemory(pNode, sizeof Node);
	else exit(EXIT_FAILURE); // Out of memory

	for (auto iTile = 0; iTile < s_nTiles; iTile++ )
		pNode->ix9State[iTile] = iTile;

	pNode->iMove = (-1);
	return pNode;
}

void Demo::EightPuzzle::Solver::Solve()
{
	Log.Grab();
	Log.Grab("Solving\n");

	pRoot = AllocNewNode();
	CopyState(ix9StartState, pRoot->ix9State);
	pRoot->iScore = s_iMaxScore;
	ToFringe(pRoot);
	
	Log.Grab();
	Log.Grab("Root\n");
	_Dbg_printout(pRoot);

	Log.Grab();
	Log.Grab("Expending nodes\n");

	while(!bSolved)
	{
		ExpendAndSearch(pRoot);
		Log.PrintAllIfAny();
	}

	Log.PrintAllIfAny();
}

INT Demo::EightPuzzle::Solver::Evaluate( _In_ IHeuristic *pEval, _In_ Node *pNode )
{
	return pEval->Evaluate(pNode);
}

void Demo::EightPuzzle::Solver::Evaluate( 
	_In_ Node *pNode 
	)
{
	pNode->iPathCost = pNode->iDepth;
	pNode->iHeuristic = Evaluate(pHeuristicEvalulator, pNode);
	pNode->iScore = pNode->iPathCost + pNode->iHeuristic;
}

Demo::EightPuzzle::Solver::Node::OnDuplicationCheck Demo::EightPuzzle::Solver::CheckForDuplication( 
	_In_ Node *pNode 
	)
{
	auto pGrandpa = pNode->pParent->pParent;
	if (!pGrandpa) return Node::OnDuplicationCheck::ZeroGrandpa;
	
	for (auto iTile = 0; iTile < s_nTiles; iTile++)
		if (pGrandpa->ix9State[iTile] != pNode->ix9State[iTile])
			return Node::OnDuplicationCheck::AtLeastOneTileIsMisplaced;

	return Node::OnDuplicationCheck::Duplicated;
}

Demo::EightPuzzle::Solver::Node * Demo::EightPuzzle::Solver::CreateAndEvaluateChild( 
	_In_ Node *pNode, INT _In_ iPossiblyLegalMove 
	)
{
	auto iHole = 0; for (auto iTile = 0; iTile < s_nTiles; iTile++)
	{
		if (pNode->ix9State[iTile] == s_iRunner)
		{
			iHole = iTile;
			break;
		}
	}

	auto iPiece = Rules::Move(iHole, iPossiblyLegalMove);
	if (Rules::IsIllegalMove(iPiece)) return nullptr;

	auto pChild = AllocNewNode();
	pChild->iDepth = ResolveDepth(pNode);
	pChild->pParent = pNode;

	CopyState(pNode->ix9State, pChild->ix9State);

	pChild->ix9State[iHole] = pNode->ix9State[iPiece];
	pChild->ix9State[iPiece] = pNode->ix9State[iHole];
	pChild->iMove = pNode->ix9State[iPiece];

	Evaluate(pChild);
	return pChild;
}

void Demo::EightPuzzle::Solver::ExpendAndSearch( _In_ Node *pNode )
{
	if (!pNode)
	{
		_ASSERT_EXPR(!pNode, "Node == nullptr at ExpendAndSearch()");
	}
	else
	{
		if (pNode->ppChildren[0] == nullptr && 
			pNode->ppChildren[1] == nullptr && 
			pNode->ppChildren[2] == nullptr && 
			pNode->ppChildren[3] == nullptr)
		{
			AllocChildren(pNode);
		}
		else
		{
			auto pTemp = vnNodeFringe.front();
			vnNodeFringe.pop_front();

			if (IsSolved(pTemp))
			{
				Log.Grab();
				Log.GrabWithArgs("Solved Nodes=%d\n", vnNodeFringe.size());
				_Dbg_printout(pTemp);
				Log.Grab();
				Log.Grab("Step-by-step solution\n");
				PrintSolution(pTemp);
				Log.Grab();
				Log.GrabWithArgs("TotalSteps=%d\n", nTotalSteps);
				Log.Grab();

				FinalizeNode(vnNodeFringe.front());
				if (bSolved) return;
			}
			else
			{
				_Dbg_printout(pTemp);
				ExpendAndSearch(pTemp);
			}
		}
	}
}

void Demo::EightPuzzle::Solver::AllocChildren( _In_ Node *pNode )
{
	for (auto iChild = 0; iChild < s_nMoves; iChild++)
	{
		auto pChild = CreateAndEvaluateChild(pNode, iChild);

		if (!pChild)
		{
			pNode->ppChildren[iChild] = nullptr;
			continue;
		}
		else if (IsSolved(pNode))
		{
			FinalizeNode(pNode);
			if (bSolved) return;
		}
		else
		{
			pNode->ppChildren[iChild] = pChild;
			pNode->nChildren++;

			if (CheckForDuplication(pChild) != Node::OnDuplicationCheck::Duplicated)
				ToFringe(pChild);
		}
	}
}

BOOL Demo::EightPuzzle::Solver::IsSolved( _In_ Node *pNode )
{
	return IsSolved(pNode->ix9State);
}

void Demo::EightPuzzle::Solver::FinalizeNode( _In_ Node *pNode )
{
	bSolved = true;
}

void Demo::EightPuzzle::Solver::_Dbg_printout( _In_ Node *pNode )
{
	static const auto _To_char = [&]( _In_ INT iTile )
	{
		switch (iTile)
		{
		case 0: return '*';
		default: return std::to_string(iTile)[0];
		}
	};

	Log.Grab();
	Log.GrabWithArgs(
		"Tiles | D=%d" "\n"
		"%c %c %c | G=%d" "\n"
		"%c %c %c | H=%d" "\n"
		"%c %c %c | F=%d" "\n",
		pNode->iDepth, 
		_To_char(pNode->ix9State[0]), _To_char(pNode->ix9State[1]), _To_char(pNode->ix9State[2]), pNode->iPathCost, 
		_To_char(pNode->ix9State[3]), _To_char(pNode->ix9State[4]), _To_char(pNode->ix9State[5]), pNode->iHeuristic, 
		_To_char(pNode->ix9State[6]), _To_char(pNode->ix9State[7]), _To_char(pNode->ix9State[8]), pNode->iScore
		);
}

void Demo::EightPuzzle::Solver::PrintSolution( _In_ Node *pNode )
{
	if (!pNode) return;
	else if (pNode->iMove != (-1))
	{
		Log.Grab();
		Log.GrabWithArgs("Expending child node Move=%d\n", pNode->iMove);
		_Dbg_printout(pNode);
	}
	else
	{
		Log.Grab();
		Log.Grab("Expending root node\n");
		_Dbg_printout(pNode);
	}

	nTotalSteps++;
	return PrintSolution(pNode->pParent);
}

#pragma region Evaluation

INT Demo::EightPuzzle::Solver::Manhattan::Evaluate( 
	_In_ Node *pNode 
	)
{
	static const INT s_Distances[9][9] =
	{
				/*0 1 2 3 4 5 6 7 8*/

		/*0*/	{ 0,1,2,1,2,3,2,3,4 }	/*0*/,
		/*1*/	{ 1,0,1,2,1,2,3,2,3 }	/*1*/,
		/*2*/	{ 2,1,0,3,2,1,4,3,2 }	/*2*/,
		/*3*/	{ 1,2,3,0,1,2,1,2,3 }	/*3*/,
		/*4*/	{ 2,1,2,1,0,1,2,1,2 }	/*4*/,
		/*5*/	{ 3,2,1,2,1,0,3,2,1 }	/*5*/,
		/*6*/	{ 2,3,4,1,2,3,0,1,2 }	/*6*/,
		/*7*/	{ 3,2,3,2,1,2,1,0,1 }	/*7*/,
		/*8*/	{ 4,3,2,3,2,1,2,1,0 }	/*8*/

				/*0 1 2 3 4 5 6 7 8*/
	};

	auto iManhattan = 0;
	for (auto iTile = 0; iTile < s_nTiles; iTile++)
		iManhattan += s_Distances[iTile][pNode->ix9State[iTile]];
	return iManhattan;
}

INT Demo::EightPuzzle::Solver::MisplaceTiles::Evaluate( 
	_In_ Node *pNode 
	)
{
	auto iMisplacedTiles = 0;
	for (auto iTile = 0; iTile < s_nTiles; iTile++) 
		if (pNode->ix9State[iTile] != iTile)
			iMisplacedTiles++;
	return iMisplacedTiles;
}

INT Demo::EightPuzzle::Solver::Nilsson::Evaluate( 
	_In_ Node *pNode 
	)
{
	Demo::EightPuzzle::Solver::Manhattan ManhattanEval;

	auto iManhattan = ManhattanEval.Evaluate(pNode);
	for (auto iTile = 0; iTile < s_nTiles - 1; iTile++)
	{
		auto const iFirst = pNode->ix9State[iTile] + 1;
		auto const iSecond = pNode->ix9State[iTile + 1];
		if ( iFirst != iSecond ) iManhattan++;
	}

	if (pNode->ix9State[s_nTiles - 1] != (s_nTiles - 1))
		iManhattan++;

	return iManhattan;
}

INT Demo::EightPuzzle::Solver::NMaxSwap::Evaluate(
	_In_ Node *pNode 
	)
{
	State ix9Temp;
	CopyState(pNode->ix9State,ix9Temp);

	auto iHeuristic = 0;
	auto iHole = 0;

	while (true)
	{
		for (auto iTile = 0; iTile < s_nTiles; iTile++)
		{
			if (!ix9Temp[iTile]) 
			{
				iHole = iTile;
				break;
			}
		}

		for (auto iTile = 0; iTile < s_nTiles; iTile++)
		{
			if (ix9Temp[iTile] == iHole) 
			{
				Swap(ix9Temp, iHole, iTile);
				iHeuristic++;
			} // if its a hole position

			if (ix9Temp[s_iRunner] == s_iRunner)
			{
				for (auto iTile = 0; iTile < s_nTiles; iTile++)
				{
					if (ix9Temp[iTile] != iTile)
					{
						Swap(ix9Temp, s_iRunner, iTile);
						iHeuristic++;
					} // if tile is misplaced
				} // foreach tile
			} // if hole is placed
		} // foreach tile

		auto nPlaced = 0;
		for (auto iTile = 0; iTile < s_nTiles; iTile++)
		{
			if (ix9Temp[iTile] == iTile)
			{
				nPlaced++;
			}
		}

		if (nPlaced == s_nTiles)
			break;
	} // infinity loop

	return iHeuristic;
}

#pragma endregion
