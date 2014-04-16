
#include <stdio.h>
#include "genocop.h"


/*
**
** void optimize( EVOLDATA	ev,
**                MATRIX	fSearchPop,
**                VECTOR	fSearchProbDist,
**                MATRIX	fRefPop,
**                VECTOR	fRefProbDist,
**                FILE *    outfile);
**
**
** Performs the main optimization of both populations.
**
*/

void optimize(
	EVOLDATA ev, MATRIX fLC, MATRIX fDC, 
	MATRIX fSearchPop, VECTOR fSearchProbDist,
	MATRIX fRefPop, VECTOR fRefProbDist, VECTOR fOpProbDist, 
	FILE *outfile, VECTOR fNormProbDist
	)
{
	float fBestRefVal;		/* best value for both max and min cases */
	float fBestSearchVal;	/* ditto for search pop */
	long lEvalCount;		/* incremented each time an eval is done */
	int iOpNum;				/* number of a selected operator */
	VECTOR fOffspring;		/* single offspring vector */
	VECTOR fBestVec;
	int i, j, iCount, iParIndex;
	long lOpFreq[GENOCOP_MAXVECTOR];
	float fRandVal;

	FILE* freqfile = 0;

	fOffspring = init_vector(ev.iNumVars + 1);
	fBestVec = init_vector(ev.iNumVars + 1);

	for (i = 1; i <= ev.iNumOperators; i++)
		lOpFreq[i] = 0L;


	if (ev.iFreqMode == GENOCOP_ADAPTIVE)
	{
		errno_t __err = fopen_s(&freqfile, "FREQ.DAT", "w");
		if (freqfile == NULL)
		{
			fclose(freqfile);
			error_exit("Frequency output file open failure. function: optimize()");
		}
	}

	/***************************************************/
	/* Initial evaluation and sorting                  */
	/***************************************************/
	/* Ref. Pop. FIRST */
	setup_rpop(ev, fRefPop);
	sort(fRefPop, ev.iObjFnType, ev.iRefPopSize);

	setup_spop(ev, fSearchPop, fRefProbDist, fRefPop, fLC, fDC);
	sort(fSearchPop, ev.iObjFnType, ev.iSearchPopSize);
	sort(fRefPop, ev.iObjFnType, ev.iRefPopSize);


	fBestRefVal = fRefPop[1][0];
	for (i = 1; i <= ev.iNumVars; i++)
		fBestVec[i] = fRefPop[1][i];

	/* fBestSearchVal = fSearchPop[1][0]; */
	lEvalCount = ev.iSearchPopSize + ev.iRefPopSize;

	fprintf(outfile, "\n=> %10ld : %10.12f\n",
		lEvalCount, fBestRefVal);

	/***************************************************/
	/* Main optimization loop starts here              */
	/***************************************************/
	while (lEvalCount <= ev.lTotEvals)
	{

		/* now, select an operator to work on the search population */
		iOpNum = select_operator(fOpProbDist);
		lOpFreq[iOpNum] = lOpFreq[iOpNum] + 1;

		/* now, operate on the parents, get the single offspring VECTOR */
		operate(ev, fSearchPop, fSearchProbDist, iOpNum,
			fDC, fLC, lEvalCount, fOffspring, ev.iSearchPopSize);

		if (check_LC(ev, fOffspring, fLC, fDC) == GENOCOP_TRUE)
		{
			/* evaluate the offspring */
			/* Here, the offspring is first copied to fSearchPop[0], which */
			/* is why that vector is RESERVED. */
			for (j = 0; j <= ev.iNumVars; j++)
				fSearchPop[0][j] = fOffspring[j];

			/*printf("Offspring is valid.\n");*/
			eval_search_vector(ev, 0, fRefProbDist, fSearchPop, fRefPop, fLC, fDC);
			lEvalCount++;

			/* after evaluating the search vector, since ref. population may */
			/* have been altered, we sort the ref. population matrix */
			sort(fRefPop, ev.iObjFnType, ev.iRefPopSize);

			/* copy offspring back from row 0 */
			for (j = 0; j <= ev.iNumVars; j++)
				fOffspring[j] = fSearchPop[0][j];

			/* always replace one of worst */
			fRandVal = randgen(0.0, 1.0);
			i = 1;
			while ((fRandVal > fRefProbDist[i]) && (i < ev.iSearchPopSize))
				i++;


			for (j = 0; j <= ev.iNumVars; j++)
				fSearchPop[ev.iSearchPopSize - i][j] = fOffspring[j];

		}
		else
		{
			/*printf("Offspring not feasible. Op. Num. %d\n", iOpNum);*/
		}

		sort(fSearchPop, ev.iObjFnType, ev.iSearchPopSize);
		sort(fRefPop, ev.iObjFnType, ev.iRefPopSize);


		/*
		**
		** If period clicks, repeat everything above for ref. pop.
		** iRefOffspring times, except that we REPLACE the parent to maintain
		** diversity.
		**
		*/
		if (ev.iRefPeriod != 0)
		{
			if ((lEvalCount % ev.iRefPeriod) == 0)
			{
				for (iCount = 1; iCount <= ev.iRefOffspring; iCount++)
				{
					/* select an operator to work on the reference population */
					iOpNum = select_operator(fOpProbDist);
					lOpFreq[iOpNum] = lOpFreq[iOpNum] + 1;
					/* now, operate on the parents, get the single offspring VECTOR */
					iParIndex = operate(ev, fRefPop, fRefProbDist, iOpNum,
						fDC, fLC, lEvalCount, fOffspring, ev.iRefPopSize);

					/* if feasible, evaluate the offspring and replace */
					if (check_all(ev, fOffspring, fLC, fDC) == GENOCOP_TRUE)
					{
						eval_ref_vector(ev, fOffspring);
						eval_ref_vector(ev, fRefPop[iParIndex]);
						switch (ev.iObjFnType)
						{
						case GENOCOP_MAX:
							if (fOffspring[0] > fRefPop[iParIndex][0])
							{
								/* copy the vector into the parent */
								for (i = 0; i <= ev.iNumVars; i++)
									fRefPop[iParIndex][i] = fOffspring[i];
								/*printf("iOpNum %d; ", iOpNum);*/

								if (ev.iFreqMode == GENOCOP_ADAPTIVE)
									/* reinitialize probability distribution */
									reassign_probab(fNormProbDist, iOpNum,
									ev.iNumOperators, GENOCOP_UP);
							}
							else
							{
								if (ev.iFreqMode == GENOCOP_ADAPTIVE)
									/* reinitialize probability distribution */
									reassign_probab(fNormProbDist, iOpNum,
									ev.iNumOperators, GENOCOP_DOWN);
							}

							if (ev.iFreqMode == GENOCOP_ADAPTIVE)
							{
								for (i = 1; i <= ev.iNumOperators; i++)
									fOpProbDist[i] = fNormProbDist[i];
								fprintf(freqfile, "Eval. # %10ld : ", lEvalCount);
								write_vector(fOpProbDist, freqfile, ev.iNumOperators);
								/*print_vector(ev.iNumOperators, fOpProbDist);*/
								normalize_opdist(fOpProbDist, ev.iNumOperators);
								/* get cumulative probability */
								cum_opdist(fOpProbDist, ev.iNumOperators);
							}
							break;
						case GENOCOP_MIN:
							if (fOffspring[0] < fRefPop[iParIndex][0])
							{
								for (i = 0; i <= ev.iNumVars; i++)
									fRefPop[iParIndex][i] = fOffspring[i];
								if (ev.iFreqMode == GENOCOP_ADAPTIVE)
									/* reinitialize probability distribution */
									reassign_probab(fNormProbDist, iOpNum,
									ev.iNumOperators, GENOCOP_UP);
							}
							else
							{
								if (ev.iFreqMode == GENOCOP_ADAPTIVE)
									/* reinitialize probability distribution */
									reassign_probab(fNormProbDist, iOpNum,
									ev.iNumOperators, GENOCOP_DOWN);
							}

							if (ev.iFreqMode == GENOCOP_ADAPTIVE)
							{
								for (i = 1; i <= ev.iNumOperators; i++)
									fOpProbDist[i] = fNormProbDist[i];
								fprintf(freqfile, "Eval. # %10ld : ", lEvalCount);
								write_vector(fOpProbDist, freqfile, ev.iNumOperators);
								/*print_vector(ev.iNumOperators, fOpProbDist);*/
								normalize_opdist(fOpProbDist, ev.iNumOperators);
								/* get cumulative probability */
								cum_opdist(fOpProbDist, ev.iNumOperators);
							}
							break;
						default:
							break;
						}

						lEvalCount++;
					}
					else
					{
						/*printf("Try # %d : offspring was not valid.\n", iCount);*/
					}
				} /* end of for no. of offspring */
			} /* end of if step matches */
			sort(fRefPop, ev.iObjFnType, ev.iRefPopSize);
		} /* end of if step = 0 */


		switch (ev.iObjFnType)
		{
		case GENOCOP_MAX:
			if (fRefPop[1][0] > fBestRefVal)
			{
				fBestRefVal = fRefPop[1][0];
				for (i = 1; i <= ev.iNumVars; i++)
					fBestVec[i] = fRefPop[1][i];
				fprintf(outfile, "=> %10ld : %10.12f\n",
					lEvalCount, fBestRefVal);
				printf("=> %10ld : %10.12f\n",
					lEvalCount, fBestRefVal);
			}
			break;
		case GENOCOP_MIN:
			if (fRefPop[1][0] < fBestRefVal)
			{
				fBestRefVal = fRefPop[1][0];
				for (i = 1; i <= ev.iNumVars; i++)
					fBestVec[i] = fRefPop[1][i];
				fprintf(outfile, "=> %10ld : %10.12f\n",
					lEvalCount, fBestRefVal);
				printf("=> %10ld : %10.12f\n",
					lEvalCount, fBestRefVal);
			}
			break;
		default:
			break;
		} /* end of case */

		/* fBestSearchVal = fSearchPop[1][0]; */

	} /* END OF MAIN OPTIMIZATION LOOP */


	fprintf(outfile,
		"\n-- BEST VECTOR FOUND --------------------------------------\n");
	for (i = 1; i <= ev.iNumVars; i++)
		fprintf(outfile, "%10.12f    ", fBestVec[i]);

	fprintf(outfile,
		"\n-- ACTUAL OPERATOR FREQUENCY ------------------------------\n");
	for (i = 1; i <= ev.iNumOperators; i++)
		fprintf(outfile, "Operator #%d was used %ld times\n", i, lOpFreq[i]);

	free_vector(fOffspring);
	free_vector(fBestVec);

	if (ev.iFreqMode == GENOCOP_ADAPTIVE)
		fclose(freqfile);

}
/**************************************************************************/


