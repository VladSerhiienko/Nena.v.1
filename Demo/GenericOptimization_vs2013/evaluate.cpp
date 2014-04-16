
#include <stdio.h>
#include "genocop.h"


/*
**
** void eval_ref_vector(EVOLDATA ev, VECTOR fV)
**
** Evaluates the supplied reference vector fV. Since reference
**vectors satisfy all constraints, all it does is evaluate the
**objective function.
**
*/
void eval_ref_vector(EVOLDATA ev, VECTOR fV)
{
	eval_func(fV, ev.iNumTestCase, ev);
}
/**************************************************************************/



/*
**
** void eval_search_vector(EVOLDATA ev, int iIndex, VECTOR fRefProbDist,
**						MATRIX fSearchPop, MATRIX fRefPop, MATRIX fLC,
**						MATRIX fDC)
**
** Evaluates a search vector in the search population, whose index
** is supplied to the routine (this should always be 0, the reserved
** vector in the search population, which is initialized by copy
** before calling this routine.
**
** This is much more complex since it might replace values in
** BOTH search and reference populations
**
*/
void eval_search_vector(EVOLDATA ev, int iIndex, VECTOR fRefProbDist,
						MATRIX fSearchPop, MATRIX fRefPop, MATRIX fLC,
							MATRIX fDC)
{
	int i, j, k, iSelIndex;
	float fRandVal;
	float fTempVec[GENOCOP_MAXVECTOR];
	int	iFlag;


	/* first, do a simple evaluation and get a value in vector[0] */
	eval_func(fSearchPop[iIndex], ev.iNumTestCase, ev);
	
	/* now, pick a reference vector using GENOCOP_RANDOM or ORDERED */
	if (ev.iSelRefPoint == GENOCOP_RANDOM)
	{
		iSelIndex = (int) randgen(1.0, (float)ev.iRefPopSize+1);
	}
	else
	{	
	fRandVal = randgen(0.0, 1.0);
	k=1;
	while (fRandVal >= fRefProbDist[k])
		k++;
	if (k>=ev.iRefPopSize) k = 1;	/* pick the best if past */
	iSelIndex = k;
	}
	

	/* move from search to reference */
	k = 0; iFlag = GENOCOP_FALSE; fRandVal = 0.0;

	if (ev.iRepairMethod == GENOCOP_RANDOM)
	do
	{
		fRandVal = randgen(0.0, 1.0);
		for (j=1; j<=ev.iNumVars; j++)
			fTempVec[j] = fRandVal*fSearchPop[iIndex][j] +
							(1-fRandVal)*fRefPop[iSelIndex][j];
		iFlag = check_all(ev, fTempVec, fLC, fDC);
		k++;
	} while ((iFlag == GENOCOP_FALSE) && (k < 100));
	else
	if (ev.iRepairMethod == GENOCOP_DETERMINISTIC)
		do
		{
			fRandVal = 1.0/power(2.0, k);
			for (j=1; j<=ev.iNumVars; j++)
				fTempVec[j] = fRandVal*fSearchPop[iIndex][j] +
								(1-fRandVal)*fRefPop[iSelIndex][j];		
			iFlag = check_all(ev, fTempVec, fLC, fDC);
			k++;
		} while ((iFlag == GENOCOP_FALSE) && (k < 20));
				
	/* if we got no feasible Z, Z = Y, eval(X)=eval(Y), maybe X=Y*/
	if (iFlag == GENOCOP_FALSE)
	{
		for (j=0; j<=ev.iNumVars; j++)
			fTempVec[j] = fRefPop[iSelIndex][j];
			
		/* now, eval(X) = eval(Y)  */
		fSearchPop[iIndex][0] =  fTempVec[0];
		
		/* if probability allows, X = Y */
		fRandVal = randgen(0.0, 1.0);
		if (fRandVal < ev.fReplRatio)
			for (j=0; j<=ev.iNumVars; j++)
				fSearchPop[iIndex][j] = fTempVec[j];	
	}
	else
	{
		/* now, evaluate Z - fully feasible, so simple evaluation */
		eval_func(fTempVec, ev.iNumTestCase, ev);
	
		/* if Z better than Y, replace Y by Z */
		switch(ev.iObjFnType)
		{
			case GENOCOP_MAX:
				if (fTempVec[0] >  fRefPop[iSelIndex][0])
				{

					for (j=0; j<=ev.iNumVars; j++)
						fRefPop[iSelIndex][j] = fTempVec[j];
				}
				break;
			case GENOCOP_MIN:
				if (fTempVec[0] <  fRefPop[iSelIndex][0])
				{

					for (j=0; j<=ev.iNumVars; j++)
						fRefPop[iSelIndex][j] = fTempVec[j];
				}
				break;
			default:
				break;
		}
		
		/* eval(X) = eval(Z), maybe X = Z */
		fSearchPop[iIndex][0] = fTempVec[0];
		
		/* if probability allows, replace X by Z as well */
		fRandVal = randgen(0.0, 1.0);
		if (fRandVal < ev.fReplRatio)
			for (j=0; j<=ev.iNumVars; j++)
				fSearchPop[iIndex][j] = fTempVec[j]; 
	} /* end of else */
}
/**************************************************************************/



