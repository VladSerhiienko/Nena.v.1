
#include <stdio.h>
#include "genocop.h"


/*
**
** void u_mutate(EVOLDATA ev, VECTOR fV, int iIndex, MATRIX fDC, MATRIX fLC)
**
** Uniform mutation operator - single parent, single offspring
**
**
*/
void u_mutate(EVOLDATA ev, VECTOR fV, int iIndex, MATRIX fDC, MATRIX fLC)
{
	int i, j, iFlag;
	float fLlim, fUlim, fSum, fNewLimit;

	iFlag = GENOCOP_FALSE;

	/* see if the chosen variable has any Domain Constraints */
	for (i = 1; i <= ev.iNumDC; i++)
	{
		if ((int) fDC[i][2] == iIndex)
		{
			fUlim = fDC[i][3];
			fLlim = fDC[i][1];
			iFlag = GENOCOP_TRUE;
		}
	}
	if (iFlag == GENOCOP_FALSE)
	{

		fUlim = GENOCOP_MAXNUM;
		fLlim = GENOCOP_MINNUM;
	}

	/* now, get new limit from linear constraints */
	/* for this, first substitute all other variables */
	/* keeping in mind that the function is stated as f[x] >= 0 */

	for (i = 1; i <= ev.iNumLC; i++)
	{
		/*
		** CHECK LIMITS ONLY IF COEFF. OF VARIABLE IS NON-ZERO
		*/
		if (fLC[i][iIndex] != 0.0)
		{
			fSum = 0.0;
			for (j = 1; j <= ev.iNumVars; j++)
			if (j != iIndex)
				fSum = fSum + fV[j] * fLC[i][j];
			fSum = fSum + fLC[i][ev.iNumVars + 1];	/* constant term */
			/* now, function is fSum+fV[iIndex]*fLC[i][iIndex] >= 0 */
			fNewLimit = (-1 * fSum);		/* fSum carried over */

			fNewLimit = fNewLimit / fLC[i][iIndex];

			/* now, if the coeff. was NEGATIVE, the inequality is CHANGED */
			/* while carrying it over, and it becomes a possible new */
			/* LOWER limit */
			if (fLC[i][iIndex] > 0.0)	/* CHANGE SIGN IF NEGATIVE */
			{
				if (fNewLimit > fLlim)
					fLlim = fNewLimit;
			}
			else
			{
				if (fNewLimit < fUlim)
					fUlim = fNewLimit;
			}
		}
	}

	/* now, generate a value within the limit i.e. mutate */
	fV[iIndex] = randgen(fLlim, fUlim);

}
/***********************************************************************=
***/

/*
**
** void b_mutate(EVOLDATA ev, VECTOR fV, int iIndex, MATRIX fDC, MATRIX =
fLC)
**
** Boundary mutation operator - single parent, single offspring
**
**
*/
void b_mutate(EVOLDATA ev, VECTOR fV, int iIndex, MATRIX fDC, MATRIX fLC)
{
	int i, j, iFlag;
	float fLlim, fUlim, fSum, fNewLimit, fRandVal;

	iFlag = GENOCOP_FALSE;
	/* see if the chosen variable has any Domain Constraints */
	for (i = 1; i <= ev.iNumDC; i++)
	{
		if ((int) fDC[i][2] == iIndex)
		{
			fUlim = fDC[i][3];
			fLlim = fDC[i][1];
			iFlag = GENOCOP_TRUE;
		}
	}
	if (iFlag == GENOCOP_FALSE)
	{

		fUlim = GENOCOP_MAXNUM;
		fLlim = GENOCOP_MINNUM;
	}

	/* now, get new limit from linear constraints */
	/* for this, first substitute all other variables */
	/* keeping in mind that the function is stated as f[x] >= 0 */

	for (i = 1; i <= ev.iNumLC; i++)
	{
		/*
		** CHECK LIMITS ONLY IF COEFF. OF VARIABLE IS NON-ZERO
		**
		*/
		if (fLC[i][iIndex] != 0.0)
		{
			fSum = 0.0;
			for (j = 1; j <= ev.iNumVars; j++)
			if (j != iIndex)
				fSum = fSum + fV[j] * fLC[i][j];
			fSum = fSum + fLC[i][ev.iNumVars + 1];	/* constant term */
			/* now, function is fSum+fV[iIndex]*fLC[i][iIndex] >= 0 */
			/* this is the new lower limit, after comparison */
			fNewLimit = (-1 * fSum) / fLC[i][iIndex];

			/* now, if the coeff. was NEGATIVE, the inequality is CHANGED */
			/* while carrying it over, and it becomes a possible new */
			/* LOWER limit */
			if (fLC[i][iIndex] > 0.0)	/* CHANGE SIGN IF NEGATIVE */
			{
				if (fNewLimit > fLlim)
					fLlim = fNewLimit;
			}
			else
			{
				if (fNewLimit < fUlim)
					fUlim = fNewLimit;
			}
		}

	}

	/* pick either the max or min. limit */
	fRandVal = randgen(0.0, 1.0);
	if (fRandVal < 0.5)
		fV[iIndex] = fLlim;
	else
		fV[iIndex] = fUlim;
}
/***********************************************************************=
***/




/*
**
** void nu_mutate(EVOLDATA ev, VECTOR fV, int iIndex,
**						MATRIX fDC, MATRIX fLC, long lNumEvals, int iPopSize)
**
** Non-uniform mutation operator - single parent, single offspring
**
**
*/
void nu_mutate(EVOLDATA ev, VECTOR fV, int iIndex,
	MATRIX fDC, MATRIX fLC, long lNumEvals, int iPopSize)
{
	int i, j, iFlag;
	float fLlim, fUlim, fSum, fNewLimit, fRandVal, fFactor;
	int iNewt, iNewT, iExponent;

	iFlag = GENOCOP_FALSE;
	/* see if the chosen variable has any Domain Constraints */
	for (i = 1; i <= ev.iNumDC; i++)
	{
		if ((int) fDC[i][2] == iIndex)
		{
			fUlim = fDC[i][3];
			fLlim = fDC[i][1];
			iFlag = GENOCOP_TRUE;
		}
	}
	if (iFlag == GENOCOP_FALSE)
	{
		fUlim = GENOCOP_MAXNUM;
		fLlim = GENOCOP_MINNUM;
	}

	/* now, get new limit from linear constraints */
	/* for this, first substitute all other variables */
	/* keeping in mind that the function is stated as f[x] >= 0 */

	for (i = 1; i <= ev.iNumLC; i++)
	{
		/*
		** CHECK LIMITS ONLY IF COEFF. OF VARIABLE IS NON-ZERO
		**
		*/
		if (fLC[i][iIndex] != 0.0)
		{
			fSum = 0.0;
			for (j = 1; j <= ev.iNumVars; j++)
			if (j != iIndex)
				fSum = fSum + fV[j] * fLC[i][j];
			fSum = fSum + fLC[i][ev.iNumVars + 1];	/* constant term */
			/* now, function is fSum+fV[iIndex]*fLC[i][iIndex] >= 0 */
			/* this is the new lower limit, after comparison */
			fNewLimit = (-1 * fSum) / fLC[i][iIndex];
			/* now, if the coeff. was NEGATIVE, the inequality is CHANGED */

			/* while carrying it over, and it becomes a possible new */
			/* LOWER limit */
			if (fLC[i][iIndex] > 0.0)	/* CHANGE SIGN IF NEGATIVE */
			{
				if (fNewLimit > fLlim)
					fLlim = fNewLimit;
			}
			else
			{
				if (fNewLimit < fUlim)
					fUlim = fNewLimit;
			}
		}

	}

	fRandVal = randgen(0.0, 1.0);
	/* pick either the max or min. limit */
	if (fRandVal < 0.5)		/* lower */
	{
		iNewt = (int) (lNumEvals / iPopSize);
		iNewT = (int) (ev.lTotEvals / iPopSize);
		iExponent = (int) (6 * randgen(0.0, 1.0)) + 1;
		fRandVal = randgen(0.0, 1.0);


		/* NOTE: The three statements below are alternate techniques  */
		/*       Suitability seems to vary according to the objective */
		/*       function.  Experiment by commenting in/out           */

		/*fFactor = power((1.0F - (float)(iNewt/iNewT)), iExponent) *
					randgen(0.0, 1.0);*/

		/*fFactor = power(0.5, iExponent)*randgen(0.0, 1.0);*/

		fFactor = power(0.2, 6)*randgen(0.0, 1.0);

		if (fFactor < .00001) fFactor = 0.00001;
		fFactor = fFactor * (fV[iIndex] - fLlim);
		fV[iIndex] = fV[iIndex] - fFactor;
	}
	else
	{
		iNewt = (int) lNumEvals / iPopSize;
		iNewT = (int) ev.lTotEvals / iPopSize;
		iExponent = (int) (6 * randgen(0.0, 1.0)) + 1;
		fRandVal = randgen(0.0, 1.0);

		/* NOTE: The three statements below are alternate techniques  */
		/*       Suitability seems to vary according to the objective */
		/*       function.  Experiment by commenting in/out           */

		/*fFactor = power((1.0F - (float)(iNewt/iNewT)), iExponent) *
					randgen(0.0, 1.0);*/

		/*fFactor = power(0.5, iExponent)*randgen(0.0, 1.0);*/

		fFactor = power(0.2, 6)*randgen(0.0, 1.0);


		if (fFactor < .00001) fFactor = 0.00001;
		fFactor = fFactor * (fUlim - fV[iIndex]);
		fV[iIndex] = fV[iIndex] + fFactor;
	}


}
/***********************************************************************=
***/




/*
**
** void hr_xover(EVOLDATA ev, VECTOR fV1, VECTOR fV2, VECTOR =
fOffspring)
**
** Heuristic crossover operator - double parent, single offspring
**
**
*/
void hr_xover(EVOLDATA ev, VECTOR fV1, VECTOR fV2, VECTOR fOffspring)
{

	float  fRandVal;
	int i;

	fRandVal = randgen(0.0, 1.0);

	/* assume, as passed, BETTER PARENT IS fV1 - see top-level code */

	for (i = 1; i <= ev.iNumVars; i++)
		fOffspring[i] = fRandVal*(fV1[i] - fV2[i]) + fV1[i];

	/* check for constraints is done at top level anyway */

}
/***********************************************************************=
***/





/*
**
** void gauss_mutate(EVOLDATA ev, VECTOR fV, long lNumEvals)
**
** Gaussian mutation operator - single parent, single offspring
**
**
*/
void gauss_mutate(EVOLDATA ev, VECTOR fV, long lNumEvals)
{
	float fSigma, fDelta;
	float fRandVal[GENOCOP_MAXVECTOR];
	int i, j;

	fSigma = (float) ((ev.lTotEvals - lNumEvals) / ev.lTotEvals);
	for (j = 1; j <= ev.iNumVars; j++)
	{
		/* create 12 random floating point numbers between 0 and 1 */
		for (i = 1; i <= 12; ++i)
			fRandVal[i] = randgen(0.0, 1.0);

		/* delta is the total of these 12 random numbers minus 6 */
		/* note that delta might be positive or negative */
		fDelta = fRandVal[1] + fRandVal[2] +
			fRandVal[3] + fRandVal[4] +
			fRandVal[5] + fRandVal[6] +
			fRandVal[7] + fRandVal[8] +
			fRandVal[9] + fRandVal[10] +
			fRandVal[11] + fRandVal[12] - 6.0F;

		/* mutate the j-th component of the vector X */
		fV[j] = fV[j] + fSigma*fDelta;
	}

}
/***********************************************************************=
***/


/*
**
** int pool_xover(EVOLDATA ev,
**					int iNumParents, VECTOR fProbDist, VECTOR fResult,
**					MATRIX fPop, int iPopSize)
**
**
** Picks bits from a random parent selected from a set
** and fills up the offspring vector.  Returns  index of the
** worst parent in the set.
**
*/
int pool_xover(EVOLDATA ev,
	int iNumParents, VECTOR fProbDist, VECTOR fResult,
	MATRIX fPop, int iPopSize)
{

	int i, j, iWorstParIndex, iRandVal;
	float fRandVal;
	VECTOR fParIndexVector;


	fParIndexVector = init_vector(iNumParents + 1);


	/* pick the req. no. of parents and note index of worst */
	iWorstParIndex = 1;

	for (i = 1; i <= iNumParents; i++)
	{
		fRandVal = randgen(0.0, 1.0);
		j = 1;
		while ((fRandVal > fProbDist[i]) && (j < iPopSize))
			j++;
		fParIndexVector[i] = (float) j;
		if (iWorstParIndex <= j) iWorstParIndex = j;
	}


	/* now, pick cells from each selected parent, filling up the */
	/* offspring vector till there are no bits left */

	for (i = 1; i <= ev.iNumVars; i++)
	{
		iRandVal = (int) randgen(1.0, (float) iNumParents + 1.0);
		fResult[i] = fPop[(int) fParIndexVector[iRandVal]][i];
	}

	free_vector(fParIndexVector);

	return(iWorstParIndex);
}
/***********************************************************************=
***/



/*
**
** int scatter_xover(EVOLDATA ev,
**					int iNumParents, VECTOR fProbDist, VECTOR fResult,
**					MATRIX fPop, int iPopSize)
**
** This operator :
**
** 1. Picks a set of parents.
** 2. Finds the 'central' vector by averaging the parent vectors.
** 3. Finds the worst vector among the parents.
** 4. Does a (heuristic) crossover between the worst and the central =
vector.
**
**
*/
int scatter_xover(EVOLDATA ev,
	int iNumParents, VECTOR fProbDist, VECTOR fResult,
	MATRIX fPop, int iPopSize)
{

	int i, j;
	float fRandVal;
	VECTOR fParIndexVector;		/* Indices of Parents selected */
	VECTOR fCentralVector;		/* Central vector */

	float fWorstVal; int iWorstParIndex;

	fParIndexVector = init_vector(iNumParents + 1);
	fCentralVector = init_vector(ev.iNumVars + 1);

	/* pick the req. no. of parents, and note the worst  */
	iWorstParIndex = 1;

	for (i = 1; i <= iNumParents; i++)
	{
		fRandVal = randgen(0.0, 1.0);
		j = 1;
		while ((fRandVal > fProbDist[i]) && (j < iPopSize))
			j++;
		fParIndexVector[i] = (float) j;
		if (iWorstParIndex <= j) iWorstParIndex = j;
	}

	/* now, find 'central' vector by averaging all parents */
	for (i = 1; i <= ev.iNumVars; i++)
	{
		for (j = 1; j <= iNumParents; j++)
			fCentralVector[i] = fCentralVector[i]
			+ fPop[(int) fParIndexVector[j]][i];
		fCentralVector[i] = fCentralVector[i] / iNumParents;
	}


	/* now, move from worst vector to central vector */
	/* similar to heuristic crossover */


	fRandVal = randgen(0.0, 1.0);
	for (i = 1; i <= ev.iNumVars + 1; i++)
		fResult[i] = fRandVal*(fCentralVector[i] - fPop[iWorstParIndex][i])
		+ fCentralVector[i];



	free_vector(fParIndexVector);
	free_vector(fCentralVector);

	return(iWorstParIndex);
}

/***********************************************************************=
***/


