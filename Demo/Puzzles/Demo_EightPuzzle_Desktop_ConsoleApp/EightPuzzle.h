#pragma once
#include "Pch.h"
#include "AsyncLogger.h"

#define PrintSeparator OutputDebugStringA( "------------------------------------------------------------------\n" )
#define Print OutputDebugStringA

namespace Demo
{
struct EightPuzzle
{
	typedef INT State[9];

	struct Solver
	{
		static const INT s_iMaxScore = INT_MAX;
		static const INT s_iRunner = 0;
		static const INT s_nTiles = 9;
		static const INT s_nMoves = 4;

		struct Rules
		{
			static const INT s_iIllegalMove = (-9);
			static inline BOOL IsLegalMove( _In_ INT iMove ) { return iMove != s_iIllegalMove; };
			static inline BOOL IsIllegalMove( _In_ INT iMove ) { return iMove == s_iIllegalMove; };
			static INT Move( _In_ INT iTile, _In_ INT iDirection );
		};

		struct Node
		{
			enum class OnDuplicationCheck
			{
				Duplicated = 2,
				AtLeastOneTileIsMisplaced = 1,
				ZeroGrandpa = 0
			};

			INT iPathCost;		// Path cost from root (g)	
			INT iHeuristic;		// Heuristic result (h)
			INT iScore;			// Total score (f)
			INT iMove;			// Where piece moved
			INT nChildren;		// Number of node children
			INT iDepth;			// Node depth

			Node *ppChildren[s_nMoves];		// Pointers to children
			Node *pParent;					// Pointer to parent node

			State ix9State;					// Node state
		};


		struct IHeuristic { virtual INT Evaluate( _In_ Node *pNode ) PURE; };
		struct Manhattan : public IHeuristic { virtual INT Evaluate( _In_ Node *pNode ); };
		struct NMaxSwap : public IHeuristic { virtual INT Evaluate( _In_ Node *pNode ); };
		struct MisplaceTiles : public IHeuristic { virtual INT Evaluate( _In_ Node *pNode ); };
		struct Nilsson : public IHeuristic { virtual INT Evaluate( _In_ Node *pNode ); };
		typedef std::list<Node*> NodeList; typedef NodeList::iterator NodeInterator;

		static inline void CopyState( _In_ State piSrc, _In_ State piDst ) { memcpy(piDst, piSrc, s_nTiles * sizeof INT); }
		static inline BOOL IsSolved( _In_ State piState ) { for (auto iTile = 0; iTile < s_nTiles; iTile++) if (piState[iTile] != iTile) return FALSE; return TRUE; }
		static inline void Swap( _In_ State piState, _In_ INT iSrc, INT iDst ) { return std::swap(piState[iSrc], piState[iDst]); }
			

		Solver( _In_ State piState, _In_ IHeuristic *pEval = nullptr );
		~Solver();

		static INT Evaluate( _In_ IHeuristic *pEval, _In_ Node *pNode ); 
		void Evaluate( _In_ Node *pNode ); 

		void Solve();
		void ExpendAndSearch( _In_ Node *pNode ); 

		Node *AllocNewNode();
		Node::OnDuplicationCheck CheckForDuplication( _In_ Node *pNode );
		Node *CreateAndEvaluateChild( _In_ Node *pNode, INT _In_ iPossiblyLegalMove );

		INT ResolveDepth( _In_ Node *pNode, _In_opt_ INT iDepth = 0 );
		void AllocChildren( _In_ Node *pNode );
		void Dealloc( _In_ Node *pNode = nullptr );
		void FinalizeNode( _In_ Node *pNode );
		BOOL IsSolved( _In_ Node *pNode );
		void ToFringe( _In_ Node *pNode );

		void PrintSolution( _In_ Node *pNode );


		IHeuristic *pHeuristicEvalulator;
		NodeList vnNodeFringe;
		State ix9StartState;
		INT nTotalSteps;
		BOOL bSolved;
		Node *pRoot;

		Utility::Logger Log;

	protected:

		void _Dbg_printout( _In_ Node *pNode );
	};
};
};