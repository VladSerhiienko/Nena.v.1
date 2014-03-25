#include "Pch.h"
#include "EightPuzzle.h"
#include "FifteenPuzzle.h"

void Demo8PzzlMain();
void Demo15PzzlMain();

int main()
{
	Demo8PzzlMain();
	//Demo15PzzlMain();
}


void Demo15PzzlMain()
{
	Demo::FifteenPuzzle::State PzzlState = 
	{
		1,  0,  2,  3, 
		5,  4,  6,  7,
		8,  9, 10, 11,
		12, 13, 15, 14
	};

	/*aState[0] = 7; aState[1] = 2; aState[2] = 4;
	aState[3] = 5; aState[4] = 0; aState[5] = 6;
	aState[6] = 8; aState[7] = 3; aState[8] = 1;*/

	/*PzzlState[0] = 2; PzzlState[1] = 12; PzzlState[2] = 4; PzzlState[3] = 3;
	PzzlState[4] = 11; PzzlState[5] = 9; PzzlState[6] = 6; PzzlState[7] = 7;
	PzzlState[8] = 14; PzzlState[9] = 8; PzzlState[10] = 10; PzzlState[11] = 13;
	PzzlState[12] = 5; PzzlState[13] = 1; PzzlState[14] = 15; PzzlState[15] = 0;*/

	/*PzzlState[0] = 1; PzzlState[1] = 2; PzzlState[2] = 3; PzzlState[3] = 0;
	PzzlState[4] = 4; PzzlState[5] = 5; PzzlState[6] = 6; PzzlState[7] = 7;
	PzzlState[8] = 8; PzzlState[9] = 9; PzzlState[10] = 10; PzzlState[11] = 11;
	PzzlState[12] = 12; PzzlState[13] = 13; PzzlState[14] = 14; PzzlState[15] = 15;*/

	auto Pzzl = new Demo::FifteenPuzzle::Solver(
		PzzlState, new Demo::FifteenPuzzle::Solver::NMaxSwap
		);

	Pzzl->Solve();
	delete Pzzl;
	Pzzl = NULL;
}

void Demo8PzzlMain()
{
	Demo::EightPuzzle::State PzzlState;

	/*aState[0] = 7; aState[1] = 2; aState[2] = 4;
	aState[3] = 5; aState[4] = 0; aState[5] = 6;
	aState[6] = 8; aState[7] = 3; aState[8] = 1;*/

	PzzlState[0] = 5; PzzlState[1] = 8; PzzlState[2] = 1;
	PzzlState[3] = 2; PzzlState[4] = 0; PzzlState[5] = 3;
	PzzlState[6] = 4; PzzlState[7] = 6; PzzlState[8] = 7;

	auto Pzzl = new Demo::EightPuzzle::Solver(
		PzzlState, new Demo::EightPuzzle::Solver::Manhattan
		);

	Pzzl->Solve();
	delete Pzzl;
	Pzzl = NULL;

}