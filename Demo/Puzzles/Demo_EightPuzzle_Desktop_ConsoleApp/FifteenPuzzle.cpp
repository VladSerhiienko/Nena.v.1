#include "Pch.h"
#include "FifteenPuzzle.h"

INT Demo::FifteenPuzzle::Solver::Rules::Move( _In_ INT iTile, _In_ INT iDirection )
{
	static const INT s_Rules[s_nTiles][s_nMoves] = 
	{
		/* 0*/ { 1, 4,				s_iIllegalMove, s_iIllegalMove },
		/* 1*/ { 0, 2,	5,			s_iIllegalMove },
		/* 2*/ { 1, 3,	6,			s_iIllegalMove },
		/* 3*/ { 2, 7,				s_iIllegalMove, s_iIllegalMove },
		/* 4*/ { 0, 5,	8,			s_iIllegalMove },
		/* 5*/ { 1, 4,	6, 9 },
		/* 6*/ { 2, 5,	7, 10 },
		/* 7*/ { 6, 3,	11,			s_iIllegalMove },
		/* 8*/ { 4, 9,	12,			s_iIllegalMove },
		/* 9*/ { 5, 8,	10, 13 },
		/*10*/ { 6, 9,	11, 14 },
		/*11*/ { 7, 10, 15,			s_iIllegalMove },
		/*12*/ { 8, 13,				s_iIllegalMove, s_iIllegalMove },
		/*13*/ { 9, 12, 14,			s_iIllegalMove },
		/*14*/ { 10, 13, 15,		s_iIllegalMove },
		/*15*/ { 11, 14,			s_iIllegalMove, s_iIllegalMove },
	};

	return s_Rules[iTile][iDirection];
}

Demo::FifteenPuzzle::Solver::Solver( _In_ State piState, _In_ IHeuristic *pEval ) : 
	pRoot(nullptr)
{
	CopyState(piState, ix9StartState);
	pHeuristicEvalulator = pEval ? pEval : new Manhattan;
	vnNodeFringe.clear();
	bSolved = FALSE;
	nTotalSteps = 0;
}

void Demo::FifteenPuzzle::Solver::Dealloc( _In_ Node *pNode )
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

Demo::FifteenPuzzle::Solver::~Solver()
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

INT Demo::FifteenPuzzle::Solver::ResolveDepth( _In_ Node *pNode, _In_opt_ INT iDepth )
{
	if (pNode->pParent) return ResolveDepth(pNode->pParent, ++iDepth);
	else return iDepth;
}

void Demo::FifteenPuzzle::Solver::ToFringe( Node *pNode )
{
	NodeInterator pIterator = vnNodeFringe.begin();
	while (pIterator != vnNodeFringe.end())
		if ( pNode->iScore <= (*pIterator)->iScore) break;
		else pIterator++;
	vnNodeFringe.insert(pIterator, pNode);
}

Demo::FifteenPuzzle::Solver::Node * Demo::FifteenPuzzle::Solver::AllocNewNode()
{
	auto pNode = new Node;

	if (pNode) ZeroMemory(pNode, sizeof Node);
	else exit(EXIT_FAILURE); // Out of memory

	for (auto iTile = 0; iTile < s_nTiles; iTile++ )
		pNode->ix9State[iTile] = iTile;

	pNode->iMove = (-1);
	return pNode;
}

void Demo::FifteenPuzzle::Solver::Solve()
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

INT Demo::FifteenPuzzle::Solver::Evaluate( _In_ IHeuristic *pEval, _In_ Node *pNode )
{
	return pEval->Evaluate(pNode);
}

void Demo::FifteenPuzzle::Solver::Evaluate( 
	_In_ Node *pNode 
	)
{
	pNode->iPathCost = pNode->iDepth;
	pNode->iHeuristic = Evaluate(pHeuristicEvalulator, pNode);
	pNode->iScore = pNode->iPathCost + pNode->iHeuristic;
}

Demo::FifteenPuzzle::Solver::Node::OnDuplicationCheck Demo::FifteenPuzzle::Solver::CheckForDuplication( 
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

Demo::FifteenPuzzle::Solver::Node * Demo::FifteenPuzzle::Solver::CreateAndEvaluateChild( 
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

void Demo::FifteenPuzzle::Solver::ExpendAndSearch( _In_ Node *pNode )
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

void Demo::FifteenPuzzle::Solver::AllocChildren( _In_ Node *pNode )
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

BOOL Demo::FifteenPuzzle::Solver::IsSolved( _In_ Node *pNode )
{
	return IsSolved(pNode->ix9State);
}

void Demo::FifteenPuzzle::Solver::FinalizeNode( _In_ Node *pNode )
{
	bSolved = true;
}

void Demo::FifteenPuzzle::Solver::_Dbg_printout( _In_ Node *pNode )
{
	static const auto _To_char = [&]( _In_ INT iTile )
	{
		switch (iTile)
		{
		case 0: return std::string(" *");
		default: return std::to_string(iTile);
		}
	};

	Log.Grab();
	Log.GrabWithArgs(
		"Tiles       | D=%d" "\n"
		"%-2d %-2d %-2d %-2d | G=%d" "\n"
		"%-2d %-2d %-2d %-2d | H=%d" "\n"
		"%-2d %-2d %-2d %-2d | F=%d" "\n"
		"%-2d %-2d %-2d %-2d | M=%d" "\n",
		pNode->iDepth, 
		(pNode->ix9State[ 0]), (pNode->ix9State[ 1]), (pNode->ix9State[ 2]), (pNode->ix9State[ 3]), pNode->iPathCost, 
		(pNode->ix9State[ 4]), (pNode->ix9State[ 5]), (pNode->ix9State[ 6]), (pNode->ix9State[ 7]), pNode->iHeuristic, 
		(pNode->ix9State[ 8]), (pNode->ix9State[ 9]), (pNode->ix9State[10]), (pNode->ix9State[11]), pNode->iScore,
		(pNode->ix9State[12]), (pNode->ix9State[13]), (pNode->ix9State[14]), (pNode->ix9State[15]), pNode->iMove
		);
}

void Demo::FifteenPuzzle::Solver::PrintSolution( _In_ Node *pNode )
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

INT Demo::FifteenPuzzle::Solver::Manhattan::Evaluate( 
	_In_ Node *pNode 
	)
{
	static const INT s_Distances[s_nTiles][s_nTiles] =
	{
				/*0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15*/

		/* 0*/	{ 0,1,2,3,1,2,3,4,2,3,4,5,3,4,5,6 }	/* 0*/,
		/* 1*/	{ 1,0,1,2,2,1,2,3,3,2,3,4,4,3,4,5 }	/* 1*/,
		/* 2*/	{ 2,1,0,1,2,2,1,2,4,3,2,3,5,4,3,4 }	/* 2*/,
		/* 3*/	{ 3,2,1,0,4,3,2,1,5,4,3,2,6,5,4,3 }	/* 3*/,
		/* 4*/	{ 1,2,2,4,0,1,2,3,1,2,3,4,2,3,4,5 }	/* 4*/,
		/* 5*/	{ 2,1,2,3,1,0,1,2,2,1,2,3,3,2,3,4 }	/* 5*/,
		/* 6*/	{ 3,2,1,2,2,1,0,1,3,2,1,2,4,3,2,3 }	/* 6*/,
		/* 7*/	{ 4,3,2,1,3,2,1,0,4,3,2,1,5,4,3,2 }	/* 7*/,
		/* 8*/	{ 2,3,4,5,1,2,3,4,0,1,2,3,1,2,3,4 }	/* 8*/,
		/* 9*/	{ 3,2,3,4,2,1,2,3,1,0,1,2,2,1,2,3 }	/* 9*/,
		/*10*/	{ 4,3,2,3,3,2,1,2,2,1,0,1,3,2,1,2 }	/*10*/,
		/*11*/	{ 5,4,3,2,4,3,2,1,3,2,1,0,4,3,2,1 }	/*11*/,
		/*12*/	{ 3,4,5,6,2,3,4,5,1,2,3,4,0,1,2,3 }	/*12*/,
		/*13*/	{ 4,3,4,5,3,2,3,4,2,1,2,3,1,0,1,2 }	/*13*/,
		/*14*/	{ 5,4,3,4,4,3,2,3,3,2,1,2,2,1,0,1 }	/*14*/,
		/*15*/	{ 6,5,4,3,5,4,3,2,4,3,2,1,3,2,1,0 }	/*15*/,

				/*0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15*/
	};

	auto iManhattan = 0;
	for (auto iTile = 0; iTile < s_nTiles; iTile++)
		iManhattan += s_Distances[iTile][pNode->ix9State[iTile]];
	return iManhattan;
}

INT Demo::FifteenPuzzle::Solver::MisplaceTiles::Evaluate( 
	_In_ Node *pNode 
	)
{
	auto iMisplacedTiles = 0;
	for (auto iTile = 0; iTile < s_nTiles; iTile++) 
		if (pNode->ix9State[iTile] != iTile)
			iMisplacedTiles++;
	return iMisplacedTiles;
}

INT Demo::FifteenPuzzle::Solver::Nilsson::Evaluate( 
	_In_ Node *pNode 
	)
{
	Demo::FifteenPuzzle::Solver::Manhattan ManhattanEval;

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

INT Demo::FifteenPuzzle::Solver::NMaxSwap::Evaluate(
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
