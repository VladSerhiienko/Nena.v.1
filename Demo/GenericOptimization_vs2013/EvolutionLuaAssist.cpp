#include "EvolutionLuaAssist.h"

void EvolutionaryOptimization::Solver::EvaluateObjectiveFunction(
	VECTOR X
	)
{
	using namespace vrc;
	using namespace Nena::Scripting;

	Params args, rets; 

	rets.add(0.0);
	for (int idx = 1; idx <= Data.iNumVars; idx++) args.add(X[idx]);

	callScriptFunction("evol_obj_func", &args, &rets);
	X[0] = rets.getValue<double>(0);
}

void EvolutionaryOptimization::Solver::EvaluteNonlinearConstrains(
	VECTOR nleq, 
	VECTOR nlineq, 
	VECTOR X
	)
{
	using namespace vrc;
	using namespace Nena::Scripting;

	Params argseq, argsineq;
	Params retseq, retsineq;

	for (int idx = 1; idx <= Data.iNumVars; idx++) 
		argseq.add(X[idx]), argsineq.add(X[idx]);

	for (int idx = 1; idx <= Data.iNumNLE; idx++) retseq.add(0.0);
	for (int idx = 1; idx <= Data.iNumNLIE; idx++) retsineq.add(0.0);

	if (Data.iNumNLE)
	{
		callScriptFunction("evol_obj_func_nleq", &argseq, &retseq);
		for (int idx = 1; idx <= Data.iNumNLIE; idx++)
			nleq[idx] = retseq.getValue<double>(idx - 1);
	}

	if (Data.iNumNLIE)
	{
		callScriptFunction("evol_obj_func_nlineq", &argsineq, &retsineq);
		for (int idx = 1; idx <= Data.iNumNLIE; idx++)
			nlineq[idx] = retsineq.getValue<double>(idx - 1);
	}
}

BOOL EvolutionaryOptimization::Solver::Run()
{
	using namespace vrc;
	using namespace Nena::Scripting;

	Data.solver = this;

	loadScript("evolution_solver", _scriptFile);
	luaL_openlibs(_p_state);

	Params args;
	Params rets;

	std::string outputFilename;

	callScriptFunction("__aligning_call", &args, &rets);

	// read static data

	GetVar("evolution_data.vars_count", Data.iNumVars);
	GetVar("evolution_data.nonlinear_eq_count", Data.iNumNLE);
	GetVar("evolution_data.nonlinear_ineq_count", Data.iNumNLIE);
	GetVar("evolution_data.linear_ineq_count", Data.iNumLC);
	GetVar("evolution_data.vars_domains_count", Data.iNumDC);
	callScriptFunction("__aligning_call", &args, &rets);
	GetVar("evolution_data.reference_population_size", Data.iRefPopSize);
	GetVar("evolution_data.search_population_size", Data.iSearchPopSize);
	GetVar("evolution_data.operators_count", Data.iNumOperators);
	GetVar("evolution_data.evaluations_count", Data.lTotEvals);
	GetVar("evolution_data.reference_population_period", Data.iRefPeriod);
	callScriptFunction("__aligning_call", &args, &rets);
	GetVar("evolution_data.reference_population_offspring_count", Data.iRefOffspring);
	GetVar("evolution_data.reference_point_selection_method", Data.iSelRefPoint);
	GetVar("evolution_data.search_point_repair_method", Data.iRepairMethod);
	GetVar("evolution_data.search_point_replacement_ratio", Data.fReplRatio);
	callScriptFunction("__aligning_call", &args, &rets);
	GetVar("evolution_data.reference_point_initialization_method", Data.iRefInitType);
	GetVar("evolution_data.search_point_initialization_method", Data.iSearchInitType);
	GetVar("evolution_data.objective_function_type", Data.iObjFnType);
	Data.iNumTestCase = 0xffffffff;
	GetVar("evolution_data.precision_factor", Data.fEpsilon);
	GetVar("evolution_data.random_number_generator_seed1", Data.iSeed1);
	GetVar("evolution_data.random_number_generator_seed2", Data.iSeed2);
	//GetVar("evolution_data.frequency_control_mode", Data.iFreqMode);
	Data.iFreqMode = GENOCOP_FIXED;
	GetVar("evolution_data.output_file", outputFilename);
	callScriptFunction("__aligning_call", &args, &rets);

	fRefPop = init_matrix(Data.iRefPopSize + 1, Data.iNumVars + 1);
	fRefProbDist = init_vector(Data.iRefPopSize + 1);
	fSearchPop = init_matrix(Data.iSearchPopSize + 1, Data.iNumVars + 1);
	fSearchProbDist = init_vector(Data.iSearchPopSize + 1);

	fDC = init_matrix(Data.iNumDC + 1, 4);
	fLC = init_matrix(Data.iNumLC + 1, Data.iNumVars + 2);
	fOpProbDist = init_vector(Data.iNumOperators + 1);
	fNormProbDist = init_vector(Data.iNumOperators + 1);

	start_randgen(Data.iSeed1, Data.iSeed2);
	assign_probab(fRefProbDist, Data.iRefPopSize, GENOCOP_RQFACTOR);
	assign_probab(fSearchProbDist, Data.iSearchPopSize, GENOCOP_SQFACTOR);

	// read dynamic data

	args.add(0);
	rets.add(0.0);
	rets.add(0.0);

	for (int idx = 1; idx <= Data.iNumVars; idx++)
	{
		args.setValue<int>(0u, idx);

		//if (Lua::GetVarToStack(_p_state, "evolution_data.objective_function_variable_domain_get"))
		{
			//callScriptFunction("", &args, &rets);
			callScriptFunction("evol_obj_func_var_domains", &args, &rets);

			fDC[idx][2] = idx;
			fDC[idx][1] = rets.getValue<double>(0);
			fDC[idx][3] = rets.getValue<double>(1);

			//printf_s("domain for is %d: [%f; %f]\n", (int)fDC[idx][2], fDC[idx][1], fDC[idx][3]);
			//callScriptFunction("__aligning_call", &args, &rets);
		}
	}

	Params lcrets;
	for (int j = 1; j <= Data.iNumVars + 1; j++)
		lcrets.add(0.0);

	for (int idx = 1; idx <= Data.iNumLC; idx++)
	{
		args.setValue<int>(0u, idx);
		//if (Lua::GetVarToStack(_p_state, "evolution_data.objective_function_lineqgez_get_coeffs"))
		{
			callScriptFunction("evol_obj_func_lineq", &args, &lcrets);
			//callScriptFunction("", &args, &lcrets);
			for (int j = 1; j <= Data.iNumVars + 1; j++)
				fLC[idx][j] = lcrets.getValue<double>(j);
			//callScriptFunction("__aligning_call", &args, &rets);
		}
	}

	Params operatorArgs;
	Params operatorRets;
	for (int idx = 1; idx <= Data.iNumOperators; idx++)
		operatorRets.add(0.0);
	//if (Lua::GetVarToStack(_p_state, "evolution_data.operators_probability_distribution_get"))
	{
		callScriptFunction("evol_operators_probabilities", &operatorArgs, &operatorRets);
		//callScriptFunction("", &operatorArgs, &operatorRets);
		for (int j = 1; j <= Data.iNumOperators; j++)
			fOpProbDist[j] = operatorRets.getValue<double>(j - 1),
			fNormProbDist[j] = fOpProbDist[j];

		//callScriptFunction("__aligning_call", &args, &rets);
	}

	normalize_opdist(fOpProbDist, Data.iNumOperators);
	cum_opdist(fOpProbDist, Data.iNumOperators);

	FILE *input, *output;
	errno_t __err2 = fopen_s(&output, outputFilename.c_str(), "w");

	if (output == NULL)
	{
		fclose(output);
		return FALSE;
	}

	write_static_output(Data, output);
	write_OD(Data, output, fOpProbDist);

	init_search_pop(Data, fSearchPop, fDC, fLC);
	init_ref_pop(Data, fRefPop, fDC, fLC);
	optimize(Data, fLC, fDC, fSearchPop, fSearchProbDist,
		fRefPop, fRefProbDist,
		fOpProbDist, output, fNormProbDist);

	fclose(output);

	free_vector(fOpProbDist);
	free_vector(fNormProbDist);
	free_vector(fRefProbDist);
	free_vector(fSearchProbDist);
	free_matrix(fDC, Data.iNumDC + 1);
	free_matrix(fLC, Data.iNumLC + 1);
	free_matrix(fRefPop, Data.iRefPopSize + 1);
	free_matrix(fSearchPop, Data.iSearchPopSize + 1);

	fDC = nullptr;
	fLC = nullptr;
	fRefPop = nullptr;
	fSearchPop = nullptr;
	fOpProbDist = nullptr;
	fNormProbDist = nullptr;
	fRefProbDist = nullptr;
	fSearchProbDist = nullptr;

	printf("app> press any key to exit...\n");

	getchar();
	return TRUE;
}
