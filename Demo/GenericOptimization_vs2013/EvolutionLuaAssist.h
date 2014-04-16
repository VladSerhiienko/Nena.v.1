#pragma once

#include <assert.h>
#include "genocop.h"

#include <Nena\LuaScripting.h>

namespace EvolutionaryOptimization
{
	struct Solver : public vrc::BaseScript<Solver>
	{
		std::string _scriptFile;

		EVOLDATA Data;
		MATRIX fSearchPop = nullptr;
		MATRIX fRefPop = nullptr;
		VECTOR fRefProbDist = nullptr;
		VECTOR fSearchProbDist = nullptr;
		MATRIX fDC = nullptr;
		MATRIX fLC = nullptr;
		VECTOR fOpProbDist = nullptr;
		VECTOR fNormProbDist = nullptr;

		Solver(std::string const &scriptFile) : _scriptFile(scriptFile) {}
		virtual ~Solver() {}

		void EvaluateObjectiveFunction(VECTOR vars);
		void EvaluteNonlinearConstrains(VECTOR nleq, VECTOR nlineq, VECTOR vars);

		BOOL Run();

		template <typename T> BOOL GetVar(
			std::string const &variable, T &var
			)
		{
			if (Nena::Scripting::Lua::GetVarToStack(_p_state, variable.c_str()))
				var = Nena::Scripting::Lua::GetVarFromStack<T>(_p_state);
			else return GENOCOP_FALSE;
			return GENOCOP_TRUE;
		}

	};
};