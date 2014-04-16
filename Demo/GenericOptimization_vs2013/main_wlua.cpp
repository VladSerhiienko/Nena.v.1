#include "EvolutionLuaAssist.h"

void main(int argc, char *argv [])
{
	using namespace EvolutionaryOptimization;

	std::string file = "Evolution.UserInput.lua";
	if (argc >= 2) file = argv[1];
	Solver input(file.c_str());
	input.Run();
}