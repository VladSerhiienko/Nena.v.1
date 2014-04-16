
#include <stdio.h>
#include "genocop.h"

/*
**
** void normalize_opdist(VECTOR fV, int iNumOps)
**
** Normalizes the operator probability distribution
**
*/
void normalize_opdist(VECTOR fV, int iNumOps)
{
	int i;
	float fSum;

	fSum = 0.0F;
	for (i = 1; i <= iNumOps; i++)
		fSum = fSum + fV[i];

	/* normalize */
	for (i = 1; i <= iNumOps; i++)
		fV[i] = fV[i] / fSum;

}
/**************************************************************************/

/*
**
** void cum_opdist(VECTOR fV, int iNumOps)
**
** Adds up the operator probability distribution
**
*/
void cum_opdist(VECTOR fV, int iNumOps)
{
	int i;

	/* now, add up for cumulative operation */
	for (i = 1; i < iNumOps; i++)
		fV[i + 1] = fV[i + 1] + fV[i];

}
/**************************************************************************/


/*
**
** int select_operator(VECTOR fV)
**
** Selects an operator based on the normalized probability distribution
**
*/
int select_operator(VECTOR fV)
{
	float fTempVal;
	int i;


	fTempVal = randgen(0.0, 1.0);
	i = 1;
	while (fTempVal >= fV[i])
		i++;
	/*printf("val : %f, operator #%d selected\n", fTempVal, i);  */
	return (i);


}
/**************************************************************************/



/*
**
** int operate(EVOLDATA ev, MATRIX fPop, VECTOR fProbDist, int iOpNum,
**					MATRIX fDC, MATRIX fLC, long lNumEvals)
**
** Does the selected operation, modifies the passed offspring
** vector, and returns with an index for a parent (for replacement
** if necessary at the top level) - note that this integer is useful
** only for reference population operations, since it is only there
** we check the offspring against the parent and replace only if it is
** better.
**
*/
int operate(EVOLDATA ev, MATRIX fPop, VECTOR fProbDist, int iOpNum,
	MATRIX fDC, MATRIX fLC,
	long lNumEvals, VECTOR fResult, int iPopSize)
{
	int i, j, k, iPar1, iPar2, iRandVal, iXoverPos;
	float fRandVal, fXoverParm;
	int *iTempVector;

	iTempVector = (int *) malloc((unsigned) (ev.iNumVars + 1)*sizeof(int));
	if (iTempVector == NULL)
		error_exit("Memory allocation failure.  function: operate()");


	switch (iOpNum)
	{
	case 1:
		/* whole arithmetical crossover */

		/* select two parents */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i<iPopSize))
			i++;

		iPar1 = i;

		do
		{
			fRandVal = randgen(0.0, 1.0);
			i = 1;
			while ((fRandVal > fProbDist[i]) && (i < iPopSize))
				i++;
		} while (i == iPar1);

		iPar2 = i;

		/* select parameter for xover */
		fXoverParm = randgen(0.0, 1.0);

		/* do the operation - whole arithmetical xover */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fXoverParm*fPop[iPar1][j] +
			(1 - fXoverParm)*fPop[iPar2][j];

		/* Make sure we return with the better parent for */
		/* comparison with offspring if need be - this    */
		/* is used at the top level for ref. population   */
		if (iPar2 < iPar1) iPar1 = iPar2;

		break;

	case 2:
		/* simple arithmetic xover */
		/* select two parents */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i<iPopSize))
			i++;

		iPar1 = i;

		do
		{
			fRandVal = randgen(0.0, 1.0);
			i = 1;
			while ((fRandVal > fProbDist[i]) && (i < iPopSize))
				i++;
		} while (i == iPar1);

		iPar2 = i;

		/* select parameter for xover */
		fXoverParm = randgen(0.0, 1.0);

		/* select crossing point */
		iXoverPos = (int) randgen(1.0, (float) ev.iNumVars + 1);
		/* do the operation - simple arithmetical xover */
		for (j = 1; j < iXoverPos; j++)
			fResult[j] = fPop[iPar1][j];
		for (j = iXoverPos; j <= ev.iNumVars; j++)
			fResult[j] = fXoverParm*fPop[iPar1][j] +
			(1 - fXoverParm)*fPop[iPar2][j];

		/* Make sure we return with the better parent for */
		/* comparison with offspring if need be - this    */
		/* is used at the top level for ref. population   */
		if (iPar2 < iPar1) iPar1 = iPar2;

		break;

	case 3:
		/* WHOLE uniform mutation */

		/* select a parent */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i < iPopSize))
			i++;
		iPar1 = i;

		/* copy parent over to fResult */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fPop[iPar1][j];


		/* now, perform uniform mutation for each variable */
		/* CHOOSING EACH ONE RANDOMLY */
		for (i = 1; i <= ev.iNumVars; i++)
			iTempVector[i] = 0;

		for (i = 1; i <= ev.iNumVars; i++)
		{
			do
			iRandVal = (int) randgen(1.0, (float) ev.iNumVars + 1);
			while (iTempVector[iRandVal] == 1);
			iTempVector[iRandVal] = 1;

			/* now, evaluate all the constraints and check the */
			/* limits allowed for the variable */
			u_mutate(ev, fResult, iRandVal, fDC, fLC);

		}


		break;
	case 4:
		/* boundary mutation */
		/* select a parent */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i < iPopSize))
			i++;

		iPar1 = i;

		/* copy parent over to fResult */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fPop[iPar1][j];

		/* select a variable for mutation */
		iRandVal = (int) randgen(1.0, (float) ev.iNumVars + 1);



		/* now, call the boundary mutation function */
		b_mutate(ev, fResult, iRandVal, fDC, fLC);

		break;
	case 5:
		/* nonuniform mutation */
		/* select a parent */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i < iPopSize))
			i++;

		iPar1 = i;

		/* copy parent over to fResult */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fPop[iPar1][j];

		/* select a variable for mutation */
		iRandVal = (int) randgen(1.0, (float) ev.iNumVars + 1);

		/* now, call the non-uniform mutation function */
		nu_mutate(ev, fResult, iRandVal, fDC, fLC, lNumEvals, iPopSize);

		break;

	case 6:
		/* WHOLE nonuniform mutation */
		/* select a parent */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i < iPopSize))
			i++;

		iPar1 = i;

		/* copy parent over to fResult */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fPop[iPar1][j];

		/* now, perform nonuniform mutation for each variable */
		/* CHOOSING EACH ONE RANDOMLY */
		for (i = 1; i <= ev.iNumVars; i++)
			iTempVector[i] = 0;

		for (i = 1; i <= ev.iNumVars; i++)
		{
			do
			iRandVal = (int) randgen(1.0, (float) ev.iNumVars + 1);
			while (iTempVector[iRandVal] == 1);
			iTempVector[iRandVal] = 1;

			nu_mutate(ev, fResult, iRandVal, fDC, fLC, lNumEvals, iPopSize);

		}
		break;

	case 7:
		/* HEURISTIC XOVER */
		/* select two parents */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i<iPopSize))
			i++;

		iPar1 = i;

		do
		{
			fRandVal = randgen(0.0, 1.0);
			i = 1;
			while ((fRandVal > fProbDist[i]) && (i < iPopSize))
				i++;
		} while (i == iPar1);

		iPar2 = i;

		/* call xover function with BETTER PARENT FIRST */
		if (iPar1 <= iPar2)
			hr_xover(ev, fPop[iPar1], fPop[iPar2], fResult);
		else
			hr_xover(ev, fPop[iPar2], fPop[iPar1], fResult);

		/* Make sure we return with the better parent for */
		/* comparison with offspring if need be - this    */
		/* is used at the top level for ref. population   */
		if (iPar2 < iPar1) iPar1 = iPar2;

		break;

	case 8:
		/* gaussian mutation */

		/* select a parent */
		fRandVal = randgen(0.0, 1.0);
		i = 1;
		while ((fRandVal > fProbDist[i]) && (i < iPopSize))
			i++;

		iPar1 = i;

		/* copy parent over to fResult */
		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = fPop[iPar1][j];

		/* now, perform gaussian mutation for the vector */
		gauss_mutate(ev, fResult, lNumEvals);

		break;

	case 9:
		/* Pool recombination operator */

		/* select the number of parents */
		iRandVal = GENOCOP_NUMPARENTS;

		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = 0.0;


		/* call the operator with the number of parents etc. */
		/* now, the worst parent index is returned, and replaced */
		/* at the top level for ref. pop */

		iPar1 = pool_xover(ev, iRandVal, fProbDist, fResult,
			fPop, iPopSize);


		break;

	case 10:
		/* scatter search operator */

		/* select the number of parents */
		iRandVal = GENOCOP_NUMPARENTS;

		for (j = 1; j <= ev.iNumVars; j++)
			fResult[j] = 0.0;

		/* call the operator with the number of parents etc. */
		/* now, the worst parent index is returned, and replaced */
		/* at the top level for ref. pop */

		iPar1 = scatter_xover(ev, iRandVal, fProbDist, fResult, fPop,
			iPopSize);
		break;

	default:
		break;

	} /* END of case */

	free(iTempVector);

	return(iPar1);

}
/**************************************************************************/


