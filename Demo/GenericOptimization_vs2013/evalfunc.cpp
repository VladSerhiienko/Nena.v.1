
#include <stdio.h>
//#include "genocop.h"
#include "EvolutionLuaAssist.h"

/*
**
** void eval_func(VECTOR X, int iNumTestCase, EVOLDATA ev)
**
** Evaluates the objective function, given a vector
**
** includes the file "FUNCTION.C"
**
*/
void eval_func(VECTOR X, int iNumTestCase, EVOLDATA ev)
{

	/*
	** The following two variables are for the case # 30 in the
	** file FUNCTION.C - they can be reused if needed.
	*/
	int i;
	double q1, q2, q3;

	switch (iNumTestCase)
	{

		/* DO NOT FORMAT the line below - preprocessor include directives MUST */
		/* start in the first column for most compilers.                       */
		/* Also, note that included files are not checked for dependencies     */
		/* during the make process unless explicitly specified.                */

//#include "function.inl"

	case 0xffffffff:
		ev.solver->EvaluateObjectiveFunction(X);
		break;

	default:
		error_exit("Target Function not found. function: eval_func()");
		break;
	}
}
/**************************************************************************/

