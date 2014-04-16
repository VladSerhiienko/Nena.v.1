

#include <stdio.h>
#include "genocop.h"

/*
**
** void assign_probab(VECTOR fProbVector, int iPopSize, float fQ)
**
** Initializes the given probability distribution array
**
** VECTOR	fProbVector	- the distribution array to be initialized
** int		iPopSize	- the size of the array (= population size)
** float	fQ			- the Q factor (prob. of best member)
**
*/
void assign_probab(VECTOR fProbVector, int iPopSize, float fQ)
{
	int             i;
	float          Q1;

	/* Q, Q(1-Q)^1, Q(1-Q)^2 ... Q(1-Q)^n */

	Q1 = fQ / (1 - power(1 - fQ, iPopSize));
	for (i = 1; i <= iPopSize; i++)
		fProbVector[i] = Q1 * power(1 - fQ, i - 1);

	/* cumulative probability distribution */
	for (i = 1; i < iPopSize; i++)
		fProbVector[i + 1] = fProbVector[i + 1] + fProbVector[i];
	/*	for (i = 1; i <= iPopSize; i++)
			printf("fProbVector[%d] = %f\n", i, fProbVector[i]); */
}
/**************************************************************************/





/*
**
** void reassign_probab(VECTOR fProbVector, int iOpNum, int iNumOperators,
**                                      int iDirection)
**
** Re-Initializes the given probability distribution array in case the
** probability system works ADAPTIVEly.
**
** VECTOR	fProbVector	- the distribution array to be re-initialized
** int		iOpNum	    - the number of the operator most recently used.
** int		iNumOperators - the total number of operators currently used.
** int		iDirection - GENOCOP_UP if the last operator improved things, GENOCOP_DOWN if not.
**
*/
void reassign_probab(VECTOR fProbVector, int iOpNum, int iNumOperators,
	int iDirection)
{
	int i, j, iCount;

	iCount = 0;


	if (iDirection == GENOCOP_UP)
	{
		if (fProbVector[iOpNum] + GENOCOP_INCREMENT <= GENOCOP_U_FREQ_LIM)
			fProbVector[iOpNum] = fProbVector[iOpNum] + GENOCOP_INCREMENT;
	}

	if (iDirection == GENOCOP_DOWN)
	{
		if (fProbVector[iOpNum] - GENOCOP_INCREMENT >= GENOCOP_L_FREQ_LIM)
			fProbVector[iOpNum] = fProbVector[iOpNum] - GENOCOP_INCREMENT;
	}

}
/**************************************************************************/





/*
**
** void init_search_pop(EVOLDATA ev, MATRIX fSearchPop, MATRIX fDC, MATRIX fLC)
**
** Initializes the search population with linearly feasible values.
** Asks the user for input if feasible values cannot be generated
** after NTRIES times.
**
** EVOLDATA	ev			- the main EVOLDATA block
** MATRIX	fSearchPop	- The search population matrix to be filled in
** MATRIX	fDC			- the domain constraint matrix
** MATRIX	fLC			- the linear constraints (inequalities) matrix
**
*/
void init_search_pop(EVOLDATA ev, MATRIX fSearchPop, MATRIX fDC, MATRIX fLC)
{

	int iFlag, iDCFlag;
	int	iPcount = 0;
	int	iTries = 1;
	int i, j, k, iNumCopies;
	char ch;



	/***************************************************/
	/* SINGLE point initalization                      */
	/***************************************************/
	if (ev.iSearchInitType == GENOCOP_SINGLE)
	{
		iFlag = GENOCOP_FALSE; iTries = 1;
		while ((iFlag == GENOCOP_FALSE) && (iTries <= GENOCOP_NTRIES))
		{
			/* generate random numbers based on the domain constraints */
			for (j = 1; j <= ev.iNumVars; j++)
			{
				iDCFlag = GENOCOP_FALSE;

				/* check if there is a DC for the variable */
				for (k = 1; k <= ev.iNumDC; k++)
				if (j == (int) fDC[k][2])
				{
					fSearchPop[1][j] = randgen(fDC[k][1], fDC[k][3]);
					iDCFlag = GENOCOP_TRUE;
				}

				/* if no DC, generate values between limits */
				if (iDCFlag == GENOCOP_FALSE)
					fSearchPop[1][j] = randgen(GENOCOP_MINNUM, GENOCOP_MAXNUM);

			} /* end of j loop*/

			/* now, check for feasibility wrt. linear inequalities */
			iFlag = check_LC(ev, fSearchPop[1], fLC, fDC);
			iTries++;
		} /* end of while */

		/* if generation failed after NTRIES times, ask user for input */
		if (iTries > GENOCOP_NTRIES)
		{
			printf("\nSearch Population : Single Point Initialization.\n");
			printf("Couldn't find a feasible vector in %d tries.\n",
				GENOCOP_NTRIES);
			iFlag = GENOCOP_FALSE;
			while (iFlag == GENOCOP_FALSE)
			{
				ask_user(fSearchPop[1], ev.iNumVars);
				iFlag = check_LC(ev, fSearchPop[1], fLC, fDC);
			};
		};

		/*
		** now, we've got a valid vector.  Copy it into the entire
		** search population.
		*/
		for (i = 2; i <= ev.iSearchPopSize; i++)
		for (j = 1; j <= ev.iNumVars; j++)
			fSearchPop[i][j] = fSearchPop[1][j];
	} /* end of if SINGLE */

	else

		/***************************************************/
		/* MULTIPLE point initalization                    */
		/***************************************************/
	{
		iPcount = 1;
		while (iPcount <= ev.iSearchPopSize)
		{
			iFlag = GENOCOP_FALSE; iTries = 1;
			while ((iFlag == GENOCOP_FALSE) && (iTries <= GENOCOP_NTRIES))
			{
				/* generate random numbers based on the domain constraints */
				for (j = 1; j <= ev.iNumVars; j++)
				{
					iDCFlag = GENOCOP_FALSE;
					/* check if there is a DC for the variable */
					for (k = 1; k <= ev.iNumDC; k++)
					if (j == (int) fDC[k][2])
					{
						fSearchPop[iPcount][j] = randgen(fDC[k][1], fDC[k][3]);
						iDCFlag = GENOCOP_TRUE;
					}

					/* if no DC, generate values between limits */
					if (iDCFlag == GENOCOP_FALSE)
						fSearchPop[iPcount][j] = randgen(GENOCOP_MINNUM, GENOCOP_MAXNUM);

				} /* end of j loop*/
				/* now, check for feasibility wrt. linear inequalities */
				iFlag = check_LC(ev, fSearchPop[iPcount], fLC, fDC);
				iTries++;
			} /* end of while */
			/* if we got a valid vector, fine :) otherwise, beg user :( */
			if (iFlag == GENOCOP_TRUE)
				iPcount++;
			else
				/* if generation failed after NTRIES times, ask user for input */
			if (iTries > GENOCOP_NTRIES)
			{
				printf("\nSearch Population : Multiple Point Initialization.\n");
				printf("Couldn't find a feasible vector in %d tries.\n",
					GENOCOP_NTRIES);
				iFlag = GENOCOP_FALSE;
				while (iFlag == GENOCOP_FALSE)
				{
					ask_user(fSearchPop[iPcount], ev.iNumVars);
					iFlag = check_LC(ev, fSearchPop[iPcount], fLC, fDC);
				};
				/* ask if user wishes to duplicate */
				printf("Valid vector: Duplicate? (Y/N) :");
				fflush(stdin); ch = getchar();
				if ((ch == 'y') || (ch == 'Y'))
				{
					iNumCopies = 0; /* make sure user inputs value */

					do
					{
						printf("\nNo of copies (1 to %d) : ",
							ev.iSearchPopSize - iPcount + 1);
						scanf_s("%d", &iNumCopies);
					} while ((iNumCopies < 1) ||
						(iNumCopies >(ev.iSearchPopSize - iPcount + 1)));
					/* END of while input validation */

					/* now, copy vectors till iNumCopies */
					for (i = 1; i < iNumCopies; i++)
					{
						for (j = 1; j <= ev.iNumVars; j++)
						{
							fSearchPop[iPcount + 1][j] = fSearchPop[iPcount][j];
						}
						iPcount++;
					}
					iPcount++;

				} /* END of block where user requests duplication */
			}; /* END of if tries exhausted */
		}; /* END of iPcount <= iSearchPopSize */
	} /* end of MULTIPLE point initialization */
}
/**************************************************************************/


/*
**
** void init_ref_pop(EVOLDATA ev, MATRIX fRefPop, MATRIX fDC, MATRIX fLC)
**
** Initializes the reference population with FULLY feasible values.
** Asks the user for input if feasible values cannot be generated
** after NTRIES times.
**
** EVOLDATA	ev			- the main EVOLDATA block
** MATRIX	fRefPop		- The reference population matrix to be filled in
** MATRIX	fDC			- the domain constraint matrix
** MATRIX	fLC			- the linear constraints (inequalities) matrix
**
*/
void init_ref_pop(EVOLDATA ev, MATRIX fRefPop, MATRIX fDC, MATRIX fLC)
{

	int iFlag, iDCFlag;
	int	iPcount = 0;
	int	iTries;
	int i, j, k, iNumCopies;
	char ch;

	/***************************************************/
	/* SINGLE point initalization                      */
	/***************************************************/
	if (ev.iRefInitType == GENOCOP_SINGLE)
	{
		iFlag = GENOCOP_FALSE; iTries = 1;
		while ((iFlag == GENOCOP_FALSE) && (iTries <= GENOCOP_NTRIES))
		{
			/* generate random numbers based on the domain constraints */
			for (j = 1; j <= ev.iNumVars; j++)
			{
				iDCFlag = GENOCOP_FALSE;
				/* check if there is a DC for the variable */
				for (k = 1; k <= ev.iNumDC; k++)
				if (j == (int) fDC[k][2])
				{
					fRefPop[1][j] = randgen(fDC[k][1], fDC[k][3]);
					iDCFlag = GENOCOP_TRUE;
				}

				/* if no DC, generate values between limits */
				if (iDCFlag == GENOCOP_FALSE)
					fRefPop[1][j] = randgen(GENOCOP_MINNUM, GENOCOP_MAXNUM);

			} /* end of j loop*/


			/* now, check for feasibility wrt. all constraints */
			iFlag = check_all(ev, fRefPop[1], fLC, fDC);
			iTries++;
		} /* end of while */

		/* if generation failed after NTRIES times, ask user for input */
		if (iTries > GENOCOP_NTRIES)
		{
			iFlag = GENOCOP_FALSE;
			printf("\nReference Population : Single Point Initialization.\n");
			printf("Couldn't find a feasible vector in %d tries.\n",
				GENOCOP_NTRIES);
			while (iFlag == GENOCOP_FALSE)
			{
				ask_user(fRefPop[1], ev.iNumVars);
				iFlag = check_all(ev, fRefPop[1], fLC, fDC);
			};
		};

		/*
		** now, we've got a valid vector.  Copy it into the entire
		** search population.
		*/
		for (i = 1; i <= ev.iRefPopSize; i++)
		for (j = 1; j <= ev.iNumDC; j++)
			fRefPop[i][j] = fRefPop[1][j];
	} /* end of if SINGLE */

	else

		/***************************************************/
		/* MULTIPLE point initalization                    */
		/***************************************************/
	{
		iPcount = 1;
		while (iPcount <= ev.iRefPopSize)
		{
			iFlag = GENOCOP_FALSE; iTries = 1;
			while ((iFlag == GENOCOP_FALSE) && (iTries <= GENOCOP_NTRIES))
			{
				/* generate random numbers based on the domain constraints */
				for (j = 1; j <= ev.iNumVars; j++)
				{
					iDCFlag = GENOCOP_FALSE;
					/* check if there is a DC for the variable */
					for (k = 1; k <= ev.iNumDC; k++)
					if (j == (int) fDC[k][2])
					{
						fRefPop[iPcount][j] = randgen(fDC[k][1], fDC[k][3]);
						iDCFlag = GENOCOP_TRUE;
					}

					/* if no DC, generate values between limits */
					if (iDCFlag == GENOCOP_FALSE)
						fRefPop[iPcount][j] = randgen(GENOCOP_MINNUM, GENOCOP_MAXNUM);

				} /* end of j loop*/
				/* now, check for feasibility wrt. linear inequalities */
				iFlag = check_all(ev, fRefPop[iPcount], fLC, fDC);
				iTries++;
			} /* end of while */
			/* if we got a valid vector, fine :) otherwise, beg user :( */
			if (iFlag == GENOCOP_TRUE)
				iPcount++;
			else
				/* if generation failed after NTRIES times, ask user for input */
			if (iTries > GENOCOP_NTRIES)
			{
				iFlag = GENOCOP_FALSE;
				printf("\nReference Population : Multiple Point Initialization.\n");
				printf("Couldn't find a feasible vector in %d tries.\n",
					GENOCOP_NTRIES);
				while (iFlag == GENOCOP_FALSE)
				{
					ask_user(fRefPop[iPcount], ev.iNumVars);
					iFlag = check_all(ev, fRefPop[iPcount], fLC, fDC);
				};
				/* ask if user wishes to duplicate */
				printf("Valid vector: Duplicate? (Y/N) :");
				fflush(stdin); ch = getchar();
				if ((ch == 'y') || (ch == 'Y'))
				{
					iNumCopies = 0; /* make sure user inputs value */

					do
					{
						printf("\nNo of copies (1 to %d) : ",
							ev.iRefPopSize - iPcount + 1);
						scanf_s("%d", &iNumCopies);
					} while ((iNumCopies < 1) ||
						(iNumCopies >(ev.iRefPopSize - iPcount + 1)));
					/* END of while input validation */

					/* now, copy vectors till iNumCopies */
					for (i = 1; i < iNumCopies; i++)
					{
						for (j = 1; j <= ev.iNumVars; j++)
						{
							fRefPop[iPcount + 1][j] = fRefPop[iPcount][j];
						}
						iPcount++;
					}
					iPcount++;
				} /* END of block where user requests duplication */
			}; /* END of if tries exhausted */
		}; /* END of iPcount <= iRefPopSize */
	} /* end of MULTIPLE point initialization */
}
/**************************************************************************/




/*
**
** void ask_user(VECTOR fVector, int iVarCount)
**
** Ask the user for an input vector and copy it into the parameter vector.
**
**
*/
void
ask_user(VECTOR fVector, int iVarCount)
{
	int i;

	/*
	** Common for both populations - place a printf before calling
	** this function so the use knows which vector (i.e. linearly
	** feasible or fully feasible) is expected.
	*/
	/*printf("\nCouldn't find a valid vector after %d tries.\n", NTRIES);*/
	printf("Please enter valid variable values for an initial vector.\n");

	for (i = 1; i <= iVarCount; i++)
	{
		printf("Please enter variable # %d (x[%d]) : ", i, i);
		scanf_s("%f", &fVector[i]);
	}
}
/**************************************************************************/



/*
**
** void	setup_rpop(EVOLDATA ev, MATRIX fRefPop)
**
** Sets up the reference population by performing the initial evaluations.
**
**
*/
void	setup_rpop(EVOLDATA ev, MATRIX fRefPop)
{
	int i;

	for (i = 1; i <= ev.iRefPopSize; i++)
		eval_ref_vector(ev, fRefPop[i]);

}
/**************************************************************************/




/*
**
** void	setup_spop(EVOLDATA ev, MATRIX fSearchPop, VECTOR fRefProbDist,
**						MATRIX fRefPop, MATRIX fLC, MATRIX fDC)
**
** Sets up the search population by performing the initial evaluations.
**
** Assumes that ref. population is already setup and SORTED
**
*/
void setup_spop(EVOLDATA ev, MATRIX fSearchPop, VECTOR fRefProbDist,
	MATRIX fRefPop, MATRIX fLC, MATRIX fDC)
{
	int i, j, k, iSelIndex;
	float fRandVal;
	float fTempVec[GENOCOP_MAXVECTOR];
	int	iFlag;

	for (i = 1; i <= ev.iSearchPopSize; i++)
	{
		eval_func(fSearchPop[i], ev.iNumTestCase, ev);  /* simple evaluation */


		/* now, pick a reference vector at GENOCOP_RANDOM (0) or ORDERED (1) */
		if (ev.iSelRefPoint == GENOCOP_RANDOM)
			iSelIndex = (int) randgen(1.0, (float) ev.iRefPopSize);
		else
		{
			fRandVal = randgen(0.0, 1.0);
			k = 1;
			while (fRandVal >= fRefProbDist[k])
				k++;
			if (k >= ev.iRefPopSize) k = 1;
			iSelIndex = k;
		}

		/* move from search to reference */
		k = 0; iFlag = GENOCOP_FALSE; fRandVal = 0.0;

		if (ev.iRepairMethod == GENOCOP_RANDOM)
		do
		{
			fRandVal = randgen(0.0, 1.0);

			for (j = 1; j <= ev.iNumVars; j++)
				fTempVec[j] = fRandVal*fSearchPop[i][j] +
				(1 - fRandVal)*fRefPop[iSelIndex][j];

			iFlag = check_all(ev, fTempVec, fLC, fDC);
			k++;
		} while ((iFlag == GENOCOP_FALSE) && (k < 100));
		else if (ev.iRepairMethod == GENOCOP_DETERMINISTIC)
		do
		{
			fRandVal = 1.0 / power(2.0, k);
			for (j = 1; j <= ev.iNumVars; j++)
				fTempVec[j] = fRandVal*fSearchPop[i][j] +
				(1 - fRandVal)*fRefPop[iSelIndex][j];
			iFlag = check_all(ev, fTempVec, fLC, fDC);
			k++;
		} while ((iFlag == GENOCOP_FALSE) && (k < 20));

		/* if no feasible Z, Z = Y */
		if (iFlag == GENOCOP_FALSE)
		for (j = 1; j <= ev.iNumVars; j++)
			fTempVec[j] = fRefPop[iSelIndex][j];

		/* now, evaluate Z */
		eval_func(fTempVec, ev.iNumTestCase, ev);

		/* if Z better than Y, replace Y by Z */
		switch (ev.iObjFnType)
		{
		case GENOCOP_MAX:
			if (fTempVec[0] > fRefPop[iSelIndex][0])
			for (j = 0; j <= ev.iNumVars; j++)
				fRefPop[iSelIndex][j] = fTempVec[j];
			break;
		case GENOCOP_MIN:
			if (fTempVec[0] < fRefPop[iSelIndex][0])
			for (j = 0; j <= ev.iNumVars; j++)
				fRefPop[iSelIndex][j] = fTempVec[j];
			break;
		default:
			break;
		}

		/* eval(X) = eval(Z) */
		fSearchPop[i][0] = fTempVec[0];
		/* if probability allows, replace X by Z as well */
		fRandVal = randgen(0.0, 1.0);
		if (fRandVal < ev.fReplRatio)
		for (j = 0; j <= ev.iNumVars; j++)
			fSearchPop[i][j] = fTempVec[j];


	}

}

/**************************************************************************/



