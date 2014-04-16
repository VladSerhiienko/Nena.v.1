

function printf(s, ...)
	return print(s:format(...))
end

function __aligning_call() 
end

print("script> initializing evolutionary optimizer...")
print("script> evolution solver v.1.0.17701")
--printf("script> evolution solver v.1.0.17701 (%s)", os.date())
__aligning_call() 

--1> this system aims to find a global optimum (min or max) of obj function
--2> domain constrains, linear inequalities, non-linear equalities/inequalities can be specified
--3> linear equalities should be placed as non-linear
--4> system can query the user for input when it is unable to generate initial feasible values

evolution_data = {}

evolution_data.consts = {}
evolution_data.consts.max = 0
evolution_data.consts.min = 1
evolution_data.consts.fixed = 0
evolution_data.consts.single = 0
evolution_data.consts.random = 0
evolution_data.consts.ordered = 1
evolution_data.consts.adaptive = 1
evolution_data.consts.multiple = 1
evolution_data.consts.deterministic = 1

--!> here the user should define constants (or just not to change)
--!> they are self-explanatory

--!> used to check equality constraints
evolution_data.precision_factor = 0.0001
evolution_data.evaluations_count = 20000
evolution_data.search_population_size = 100
evolution_data.reference_population_size = 50
evolution_data.reference_population_period = 5000
evolution_data.reference_population_offspring_count = 10
evolution_data.search_point_replacement_ratio = 0.250

--!> must be between 0 and 31328
evolution_data.random_number_generator_seed1 = 15000
--!> must be between 0 and 30081
evolution_data.random_number_generator_seed2 = 15000

--!> here the user should define methods (or just not to change)
--!> note: please, choose from evolution_data.consts fields

--!> random/deterministic
evolution_data.search_point_repair_method = evolution_data.consts.random
--!> random/ordered (uses probability distribution)
evolution_data.reference_point_selection_method = evolution_data.consts.random
--!> single/multiple
evolution_data.reference_point_initialization_method = evolution_data.consts.single
--!> single/multiple
evolution_data.search_point_initialization_method = evolution_data.consts.multiple

function evol_obj_func(x, y) 
	--printf("objf: x=%f y=%f", x, y)
	return 2 * x * x + 4 * y * y - 2 * x * y - 3 * x + 6 * y
end

--!> this function should evaluate non-linear inequality (greater equal zero) expression
function evol_obj_func_nlineq(x, y) -- h(x) >= 0
	--printf("nlineq: i=%i x=%f y=%f", idx, x, y)

	return	(16 - 2 * x * x - y * y),	--!> 16 - 2 x^2 - y^2 >= 0
			(-math.sqrt(3) * x * x + y),		--!> -sqrt3 x^2 + y >= 0
			(x)							--!> x >= 0
end

--!> this function should evaluate non-linear equality expression
function evol_obj_func_nleq(x, y) --!> h(x) == 0
	--print("evol_obj_func_nleq")
	return 10 * (y - x * x)
end

--!> this function should return linear inequality coefficients (greater equal zero) expression
function evol_obj_func_lineq(idx) --!> g(x) >= 0
	--print("evol_obj_func_lineq")
	if (idx == 1) then return 1, 1, -1 --!> x + y - 1 >= 0
	--else print("undefined linear inequality constrain") --os.exit() 
	end
end

--!> this function should return upper and lower provided variable domain bounds
function evol_obj_func_var_domains(idx)
	if (idx == 1) then return -10, 10 --!> x min, max
	elseif (idx == 2) then return -10, 10 --!> y min, max
	else return -100, 100 -- print("undefined variable domain")
	end
end

--!> this function should return an array of operators probability distribution
--!> note: array size should correspond to evolution_data.operators_count number

-- order: 
-- 1  whole arithmetical crossover
-- 2  simple arithmetical crossover
-- 3  uniform mutation
-- 4  boundary mutation
-- 5  non-uniform mutation
-- 6  whole non-uniform mutation
-- 7  heuristic crossover
-- 8  gaussian mutation
-- 9  pool recombination operator
-- 10 scatter search operator

function evol_operators_probabilities()
	--print("evol_operators_probabilities")
	return 4, 4, 4, 4, 4, 4, 4, 4 --!> 8 operators
end

evolution_data.vars_count = 2
evolution_data.vars_domains_count = 2
evolution_data.operators_count = 8
evolution_data.linear_ineq_count = 0
evolution_data.nonlinear_eq_count = 0
evolution_data.nonlinear_ineq_count = 0
evolution_data.objective_function_type = evolution_data.consts.min

-- file to write program output into
evolution_data.output_file = "Evolution.AppOutput.txt"

--[[
]]--
