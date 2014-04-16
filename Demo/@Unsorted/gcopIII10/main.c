#include <stdio.h>
#include "genocop.h"


void
main(int argc, char *argv[])
{

	FILE *input, *output;
	
    EVOLDATA ed;			/* data block for evol. data - see GENOCOP.H */
    MATRIX fSearchPop;		/* search population matrix */
    MATRIX fRefPop;			/* reference population matrix */
    VECTOR fRefProbDist;	/* prob. dist. vector of ref. population */
    VECTOR fSearchProbDist; /* prob. dist. vector of search population */
    MATRIX fDC;				/* Domain constraint matrix */
    MATRIX fLC;				/* Linear inequalities matrix */
    VECTOR fOpProbDist; 	/* array of prob. distribution of operators */
    VECTOR fNormProbDist;		/* Normalized input probability dist. of ops */
    int i;

	int my_argc = 3;
	char *my_argv[3] = 
	{
		"module",
		"in.labwork.1",
		"out.labwork.1",
	};

	argc = my_argc;
	argv = my_argv;
    
    if (argc < 3)
    {
		printf("GENOCOP III v1.0\n");
		printf("Copyright (C) 1995 by Girish Nazhiyath, Zbigniew Michalewicz\n");
		printf("\nERROR: Invalid Command Syntax.\n\n");
		printf("Syntax is: %s <infilename> <outfilename>\n\n", argv[0]);
		exit(1);
    }
    
	/* strip comments from the input file and write to temp file */
	strip_comments(argv[1]);

	/* now, open temp file as input */
	errno_t __err = fopen_s(&input, "TEMPFILE", "r");
    if (input == NULL)	{
    	fclose(input);
		error_exit("Input file open failure. function: main()");
    }
    		
	errno_t __err2 = fopen_s(&output, argv[2], "w");
    if (output == NULL) {
    	fclose(output);
		error_exit("Output file open failure. function: main()"); 
    }
    

	
    /* read static data from input file */
    read_static_input(&ed, input);
    /* Write static data as a log to the output file */
    write_static_output(ed, output);


		    
	/***************************************************/
	/*allocate memory for vectors and matrices         */
	/***************************************************/

	/* search population */
	fSearchPop = init_matrix(ed.iSearchPopSize+1, ed.iNumVars + 1);

	/* reference population */
	fRefPop = init_matrix(ed.iRefPopSize+1, ed.iNumVars+1);

	/* probability distribution for ref. pop.  */
	fRefProbDist = init_vector(ed.iRefPopSize+1);
	fSearchProbDist = init_vector(ed.iSearchPopSize+1);

	
	/*******************************************************************/
	/* The following three make up dynamic data read from the datafile */
	/*******************************************************************/
	/* Domain constraint matrix */
	fDC = init_matrix(ed.iNumDC+1, 4);

	/* Linear Inequalities matrix [1..iNumVars] + 1 for constant */
	fLC = init_matrix(ed.iNumLC+1, ed.iNumVars+2);
	
	/* probability distribution for operators */
	fOpProbDist = init_vector(ed.iNumOperators+1);
	fNormProbDist = init_vector(ed.iNumOperators+1);
	

	/***************************************************/
	/* Initialize random number generator              */
	/***************************************************/
	start_randgen(ed.iSeed1, ed.iSeed2);	

	/* assign the probability distributions */
	assign_probab(fRefProbDist, ed.iRefPopSize, RQFACTOR);
	assign_probab(fSearchProbDist, ed.iSearchPopSize, SQFACTOR);
	

	/***************************************************/
	/* read in dynamic data from the input file        */
	/***************************************************/
    read_DC(ed, input, fDC);
    read_LC(ed, input, fLC);
    read_OD(ed, input, fOpProbDist);

	/***************************************************/
	/* write out dynamic data to the output file       */
	/***************************************************/    
    write_DC(ed, output, fDC);
    write_LC(ed, output, fLC);
    write_OD(ed, output, fOpProbDist);


	for (i=1; i<=ed.iNumOperators; i++)
		fNormProbDist[i] = fOpProbDist[i];
		
    /* normalize operator probability distribution */
    normalize_opdist(fOpProbDist, ed.iNumOperators);

    /* write the normalized distribution to the output file */
	fprintf(output,
		"\n-- NORMALIZED ---------------------------------------------");
    write_OD(ed, output, fOpProbDist);
    /* now, add up to get cumulative prob. dist */
    cum_opdist(fOpProbDist, ed.iNumOperators);
    


	/***************************************************/
	/* Initialize arrays                               */
	/***************************************************/
	/* search population */
	init_search_pop(ed, fSearchPop, fDC, fLC);


	/* reference population */
	init_ref_pop(ed, fRefPop, fDC, fLC);


	fprintf(output,
		"\n-- OPTIMIZATION -------------------------------------------\n");
	/***************************************************/
	/* optimization loop                               */
	/***************************************************/
	optimize(ed, fLC, fDC, fSearchPop, fSearchProbDist,
				fRefPop, fRefProbDist,
				fOpProbDist, output, fNormProbDist);
	


	/***************************************************/
	/* Clean up                                        */
	/***************************************************/

	fclose(input);
	fclose(output);


	/* free memory */
	free_matrix (fSearchPop, ed.iSearchPopSize + 1);
	free_matrix (fRefPop, ed.iRefPopSize + 1);
	free_vector (fRefProbDist);
	free_vector (fSearchProbDist);
	free_matrix (fDC, ed.iNumDC+1);
	free_matrix (fLC, ed.iNumLC+1);
	free_vector (fOpProbDist);
	free_vector (fNormProbDist);
	
	
    
	    
	//getchar();
    exit(0);
}
/**************************************************************************/

