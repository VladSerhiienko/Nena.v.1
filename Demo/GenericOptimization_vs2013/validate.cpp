
#include <stdio.h>
//#include "genocop.h"
#include "EvolutionLuaAssist.h"

/*
**
** int check_LC(EVOLDATA ev, VECTOR X, MATRIX fLC, MATRIX fDC)
**
** Checks for linear constraints (domain and linear equalities and
** inequalities
**
**
*/
int check_LC(EVOLDATA ev, VECTOR X, MATRIX fLC, MATRIX fDC)
{
	/* all linear constraints are of the form f[x] >= 0 */
	
	int i, j;
	int iFlag, iFlag1, iFlag2, iDCFlag;
	float fTempVal;
	
	iFlag1 = GENOCOP_TRUE; iFlag2 = GENOCOP_TRUE;
	
	/* for ANY variable, if DC is not met, iFlag1 is GENOCOP_FALSE */
	for (j=1; j<=ev.iNumVars; j++)
	{
		iDCFlag = GENOCOP_FALSE;
		/* check if there is a DC */
		for (i=1; i<=ev.iNumDC; i++)
		if ((int)fDC[i][2] == j)
		{
			iDCFlag = GENOCOP_TRUE;
			if ((X[j] < fDC[i][1]) || (X[j] > fDC[i][3]))
				iFlag1 = GENOCOP_FALSE;
		}

		/* If not, use max. limits */		
		if (iDCFlag == GENOCOP_FALSE)
		{
			if ((X[j] < GENOCOP_MINNUM) || (X[j] > GENOCOP_MAXNUM))
				iFlag1 = GENOCOP_FALSE;
		}
	}
				

	if (ev.iNumLC >= 1)
	for (i=1; i<=ev.iNumLC; i++)
	{
		fTempVal = 0.0F; 
		for (j=1; j<=ev.iNumVars; j++)
			fTempVal = fTempVal + X[j]*fLC[i][j];
		/* add the constant term here */
		fTempVal = fTempVal + fLC[i][ev.iNumVars+1];

		if (fTempVal < 0.0F)
			iFlag2 = GENOCOP_FALSE;
	}


	if ((iFlag1 == GENOCOP_TRUE) && (iFlag2 == GENOCOP_TRUE))
		iFlag = GENOCOP_TRUE;
	else
		iFlag = GENOCOP_FALSE;


	return (iFlag);
}
/**************************************************************************/



/*
**
** int check_LC(EVOLDATA ev, VECTOR X, MATRIX fLC, MATRIX fDC)
**
** Checks for all constraints - calls check_LC to check for linear
** constraints, then performs checks for nonlinear constraints,
** combines the result and returns it.
**
*/
int check_all(EVOLDATA ev, VECTOR X, MATRIX fLC, MATRIX fDC)
{


	int iFlag, i, iCount;
	int iFlag1, iFlag2, iFlag3;
	float nlineq[GENOCOP_MAXVECTOR], nleq[GENOCOP_MAXVECTOR], fQ;
	

	/******************************************************************/
	/******************* START OF DATA ENTRY AREA *********************/
	/******************************************************************/	
	
	/*
	**
	** NONLINEAR INEQUALITIES in the form f[x] >= 0
	**
	**
	*/
	switch(ev.iNumTestCase)
	{

	/* DO NOT FORMAT the line below - preprocessor include directives MUST */
	/* start in the first column for most compilers.                       */
	/* Also, note that included files are not checked for dependencies     */
	/* during the make process unless explicitly specified.                */
	
//#include "nlineq.inl"

	case 0xffffffff:
		ev.solver->EvaluteNonlinearConstrains(nleq, nlineq, X);
		break;

	default: break;
	}
	
	
	/*
	**
	** NONLINEAR EQUALITIES in the form f[x] = 0
	**
	**
	*/
	switch(ev.iNumTestCase)
	{

	/* DO NOT FORMAT the line below - preprocessor include directives MUST */
	/* start in the first column for most compilers.                       */
	/* Also, note that included files are not checked for dependencies     */
	/* during the make process unless explicitly specified.                */
	
//#include "nleq.inl"
	
	default: break;
	}	
	


	/******************************************************************/
	/******************* END OF DATA ENTRY AREA ***********************/
	/******************************************************************/


	iFlag1 = GENOCOP_TRUE; iFlag2 = GENOCOP_TRUE; iFlag3 = GENOCOP_TRUE;

	
	/* check for feasibility w.r.t. NONLINEAR INEQUALITIES */

	if (ev.iNumNLIE >= 1)
		for (i = 1; i<=ev.iNumNLIE; i++)
			if (nlineq[i] < 0.0)
				iFlag1 = GENOCOP_FALSE;
			

	/* check for feasibility w.r.t. NONLINEAR EQUALITIES */

	if (ev.iNumNLE >= 1)
		for (i=1; i<=ev.iNumNLE; i++)
		{
			if ((nleq[i] > ev.fEpsilon) || (nleq[i] < (-1.0F*ev.fEpsilon)))
				iFlag2 = GENOCOP_FALSE;
		}

	/* check for feasibility w.r.t. LINEAR (IN)EQUALITIES */
	if ((ev.iNumLC >=1) || (ev.iNumDC >=1))
			iFlag3 = check_LC(ev, X, fLC, fDC);

	if ((iFlag1 == GENOCOP_TRUE) && (iFlag2 == GENOCOP_TRUE) && (iFlag3 == GENOCOP_TRUE))
		iFlag = GENOCOP_TRUE;
	else
		iFlag = GENOCOP_FALSE;

	return (iFlag);
	
}
/**************************************************************************/

