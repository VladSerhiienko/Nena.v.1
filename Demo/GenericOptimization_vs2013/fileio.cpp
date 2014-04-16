#include <stdio.h>
#include "genocop.h"


/*
**
** void strip_comments(char *filename)
**
** Strips comments from the input file and creates TEMPFILE.
**
**
**
*/
void strip_comments(char *filename)
{

	FILE *tfile;
	FILE *fp;
	char tempstring[GENOCOP_MAXCHARS];

	errno_t __err = fopen_s(&fp, filename, "r");
	if (fp == NULL)
	{
		fclose(fp);
		error_exit("Input file open failure. function: strip_comments()");
	}


	errno_t __err2 = fopen_s(&tfile, "TEMPFILE", "w");
	if (tfile == NULL)
	{
		fclose(tfile);
		error_exit("Temporary file open failure. function: strip_comments()");
	}



	while (!feof(fp))
	{
		fgets(tempstring, GENOCOP_MAXCHARS, fp);
		if (feof(fp))
			break;
		if (tempstring[0] != '#')
		{
			fputs(tempstring, tfile);
			fprintf(tfile, "\n");
		}

	}

	fclose(fp); fclose(tfile);

}
/**************************************************************************/


/*
**
** void read_static_input(EVOLDATA *ev, FILE *fp)
**
** Reads the static input data into the data structure ev.
**
**
**
*/
void read_static_input(EVOLDATA *ev, FILE *fp)
{

	fscanf_s(fp, "%d\n", &ev->iNumVars);
	fscanf_s(fp, "%d\n", &ev->iNumNLE);
	fscanf_s(fp, "%d\n", &ev->iNumNLIE);
	fscanf_s(fp, "%d\n", &ev->iNumLC);
	fscanf_s(fp, "%d\n", &ev->iNumDC);
	fscanf_s(fp, "%d\n", &ev->iRefPopSize);
	fscanf_s(fp, "%d\n", &ev->iSearchPopSize);
	fscanf_s(fp, "%d\n", &ev->iNumOperators);
	fscanf_s(fp, "%ld\n", &ev->lTotEvals);
	fscanf_s(fp, "%d\n", &ev->iRefPeriod);
	fscanf_s(fp, "%d\n", &ev->iRefOffspring);
	fscanf_s(fp, "%d\n", &ev->iSelRefPoint);
	fscanf_s(fp, "%d\n", &ev->iRepairMethod);
	fscanf_s(fp, "%f\n", &ev->fReplRatio);
	fscanf_s(fp, "%d\n", &ev->iRefInitType);
	fscanf_s(fp, "%d\n", &ev->iSearchInitType);
	fscanf_s(fp, "%d\n", &ev->iObjFnType);
	fscanf_s(fp, "%d\n", &ev->iNumTestCase);
	fscanf_s(fp, "%f\n", &ev->fEpsilon);
	fscanf_s(fp, "%d\n", &ev->iSeed1);
	fscanf_s(fp, "%d\n", &ev->iSeed2);
	fscanf_s(fp, "%d\n", &ev->iFreqMode);
}
/**************************************************************************/

/*
**
** void write_static_output(EVOLDATA ev, FILE *fp)
**
** Writes the data structure ev to the file pointed to by fp,
**
** alongwith explanations of the fields.
**
**
*/
void write_static_output(EVOLDATA ev, FILE *fp)
{
	fprintf(fp, "\n-- GENERAL DATA -------------------------------------------\n\n");
	fprintf(fp, "No. of variables .....................: %d\n", ev.iNumVars);
	fprintf(fp, "No. of linear equalities .............: %d\n", ev.iNumNLE);
	fprintf(fp, "No. of non-linear inequalities .......: %d\n", ev.iNumNLIE);
	fprintf(fp, "No. of linear inequalities ...........: %d\n", ev.iNumLC);
	fprintf(fp, "No. of domain constraints ............: %d\n", ev.iNumDC);
	fprintf(fp, "Reference population size ............: %d\n", ev.iRefPopSize);
	fprintf(fp, "Search population size ...............: %d\n", ev.iSearchPopSize);
	fprintf(fp, "No. of operators .....................: %d\n", ev.iNumOperators);
	fprintf(fp, "Total no. of evaluations .............: %ld\n", ev.lTotEvals);
	fprintf(fp, "Reference population evolution period : %d\n", ev.iRefPeriod);
	fprintf(fp, "No. of offspring for ref. population .: %d\n", ev.iRefOffspring);
	fprintf(fp, "Reference point selection method .....: %d\n", ev.iSelRefPoint);
	fprintf(fp, "Search point repair method ...........: %d\n", ev.iRepairMethod);
	fprintf(fp, "Search point replacement ratio .......: %f\n", ev.fReplRatio);
	fprintf(fp, "Reference point initialization method : %d\n", ev.iRefInitType);
	fprintf(fp, "Search point initialization method ...: %d\n", ev.iSearchInitType);
	fprintf(fp, "Objective function type ..............: %d\n", ev.iObjFnType);
	fprintf(fp, "Test case number .....................: %d\n", ev.iNumTestCase);
	fprintf(fp, "Precision factor .....................: %f\n", ev.fEpsilon);
	fprintf(fp, "Random number generator Seed #1.......: %d\n", ev.iSeed1);
	fprintf(fp, "Random number generator Seed #2.......: %d\n", ev.iSeed2);
	fprintf(fp, "Frequency control mode................: %d\n", ev.iFreqMode);
}
/**************************************************************************/



/*
**
** void	read_DC(EVOLDATA ev, FILE *fp, MATRIX fM)
**
** Reads the domain constraints from the file into the parameter matrix.
**
**
*/
void	read_DC(EVOLDATA ev, FILE *fp, MATRIX fM)
{
	int i;

	for (i = 1; i <= ev.iNumDC; i++)
	{
		fscanf_s(fp, "%f %f %f", &fM[i][1], &fM[i][2], &fM[i][3]);
	}
}
/**************************************************************************/


/*
**
** void	read_LC(EVOLDATA ev, FILE *fp, MATRIX fM)
**
** Reads the linear constraints from the file into the parameter matrix.
**
**
*/
void    read_LC(EVOLDATA ev, FILE *fp, MATRIX fM)
{
	int i, j;

	for (i = 1; i <= ev.iNumLC; i++)
	{
		for (j = 1; j <= ev.iNumVars + 1; j++)
		{
			fscanf_s(fp, "%f", &fM[i][j]);
		}
	}
}
/**************************************************************************/


/*
**
** void	read_OD(EVOLDATA ev, FILE *fp, VECTOR fV)
**
** Reads the probability distribution of the operators
** into the parameter vector.
**
**
*/
void    read_OD(EVOLDATA ev, FILE *fp, VECTOR fV)
{
	int i;

	if (ev.iFreqMode == GENOCOP_ADAPTIVE)
	for (i = 1; i <= ev.iNumOperators; i++)
		fV[i] = GENOCOP_L_FREQ_LIM;
	else
	for (i = 1; i <= ev.iNumOperators; i++)
		fscanf_s(fp, "%f", &fV[i]);
}
/**************************************************************************/



/*
**
** void	write_DC(EVOLDATA ev, FILE *fp, MATRIX fM)
**
** Writes the domain constraints from the parameter matrix into the
** output file.
**
**
*/
void	write_DC(EVOLDATA ev, FILE *fp, MATRIX fM)
{
	int i;

	fprintf(fp,
		"\n-- DOMAIN CONSTRAINTS -------------------------------------\n\n");

	for (i = 1; i <= ev.iNumDC; i++)
	{
		fprintf(fp, "Variable # %d; Lower Limit %f; Upper Limit %f.\n",
			(int) fM[i][2], fM[i][1], fM[i][3]);
	};
	fprintf(fp, "\n");
}
/**************************************************************************/



/*
**
** void	write_LC(EVOLDATA ev, FILE *fp, MATRIX fM)
**
** Writes the linear constraints from the parameter matrix into the
** output file.
**
**
*/
void    write_LC(EVOLDATA ev, FILE *fp, MATRIX fM)
{
	int i, j;

	fprintf(fp,
		"\n-- LINEAR CONSTRAINTS -------------------------------------\n\n");

	for (i = 1; i <= ev.iNumLC; i++)
	{
		fprintf(fp, "# %d : ", i);
		for (j = 1; j <= ev.iNumVars; j++)
		{
			fprintf(fp, "%f*X[%d] +  ", fM[i][j], j);
		}
		fprintf(fp, "%f >= 0.0 \n", fM[i][ev.iNumVars + 1]);
	};
	fprintf(fp, "\n");

}
/**************************************************************************/





/*
**
** void	write_OD(EVOLDATA ev, FILE *fp, VECTOR fV)
**
** Writes the operator probability distribution
** from the parameter vector into the output file.
**
**
*/
void    write_OD(EVOLDATA ev, FILE *fp, VECTOR fV)
{
	int i;

	fprintf(fp,
		"\n-- OPERATOR DATA ------------------------------------------\n\n");
	for (i = 1; i <= ev.iNumOperators; i++)
		fprintf(fp, "Operator # %d : %f\n", i, fV[i]);
}
/**************************************************************************/



/*
**
** void	write_DC(EVOLDATA ev, FILE *fp, MATRIX fM)
**
** Writes the parameter vector into the
** output file.
**
**
*/
void write_vector(VECTOR fV, FILE* fp, int iLength)
{
	int i;

	for (i = 1; i <= iLength; i++)
		fprintf(fp, "%10.12f  ", fV[i]);
	fprintf(fp, "\n");
}
/**************************************************************************/
