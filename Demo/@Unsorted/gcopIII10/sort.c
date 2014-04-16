
#include <stdio.h>
#include "genocop.h"


/*
**
** void sort(MATRIX fPop, int iSortType, int iPopSize)
**
** Sorts a supplied population in ascending or descending order 
**
*/
void sort(MATRIX fPop, int iSortType, int iPopSize)
{
    int             i, j;

	/* in both cases, MAX and MIN, the best value is on top */
    switch (iSortType)
	{
    case MIN:
	for (i = 1; i <= iPopSize; i++)
	    for (j = i + 1; j <= iPopSize; j++)
		if (fPop[i][0] > fPop[j][0])
		    swap(&fPop[i], &fPop[j]);
	break;

    case MAX:
	for (i = 1; i <= iPopSize; i++)
	    for (j = i + 1; j <= iPopSize; j++)
		if (fPop[i][0] < fPop[j][0])
		    swap(&fPop[i], &fPop[j]);
	break;
    default:
	error_exit("Incorrect Data (must be 0 or 1). function: sort()");

	}
}
/**************************************************************************/


/*
**
** void swap(float **x, float **y)
**
** Swaps pointers - used by function sort()
**
*/
void
swap(float **x, float **y)
{
    float          *temp;

    temp = *x;
    *x = *y;
    *y = temp;
}
/**************************************************************************/



/*
**
** void insert_vector(MATRIX fPop, VECTOR fV, int iSortType, int iPopSize,
**					int iNVars)
**
** Inserts a vector into an already sorted population, maintaining
** the sorted distribution.
**
*/
void insert_vector(MATRIX fPop, VECTOR fV, int iSortType, int iPopSize,
					int iNVars)
{
	int i, j, iInsPoint;

	i = 1;
	switch(iSortType)
	{
		case MAX:

			while ((fV[0]<fPop[i][0]) && (i<iPopSize))
					i++; /* get to the right ins. point */
			iInsPoint = i;
			/* if not better than worst, return */
			if ((iInsPoint > iPopSize) || (iInsPoint < 1))
			{
				/*printf("INSERT NOT DONE\n");*/
				return;
			}

			/* now, shift all vectors from InsPoint down by one row */
			for (i=iPopSize; i>iInsPoint; i--)
				for (j=0; j<=iNVars; j++)
					fPop[i][j] = fPop[i-1][j];
			
			/* insert our vector */
			for (j=0; j<=iNVars; j++)
					fPop[iInsPoint][j] = fV[j];
		
		case MIN:
		
			while ((fV[0]>fPop[i][0]) && (i<iPopSize))
				i++; /* get to the right ins. point */
			iInsPoint = i;
			
			/* if not better than worst, return */
			if ((iInsPoint > iPopSize) || (iInsPoint < 1))
			{
				/*printf("INSERT NOT DONE: fV[0] = %f; fPop[%d] = %f\n",
								fV[0], iPopSize, fPop[iPopSize][0]); */
				return;
			}
	/*printf("MIN. INSERT OK. point %d, iPopSize %d, vector=%f, inspointval=%f, prevval=%f\n",
					iInsPoint, iPopSize, fV[0], fPop[iInsPoint][0],
						fPop[iInsPoint-1][0]);*/
			
			if (fV[0] < fPop[iInsPoint][0])
			{
			/* now, shift all vectors from InsPoint down by one row */
			for (i=iPopSize; i>iInsPoint; i--)
				for (j=0; j<=iNVars; j++)
					fPop[i][j] = fPop[i-1][j];
			
			/* insert our vector */
			for (j=0; j<=iNVars; j++)
					fPop[iInsPoint][j] = fV[j];	
			}
			else
			{
				/*printf("INSERT NOT DONE (EQUAL): fV[0] = %f; fPop[%d] = %f\n",
								fV[0], iInsPoint, fPop[iInsPoint][0]);*/
			
				return;			
			}
				

	}	/* END of switch */
}
/**************************************************************************/


	
