
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define GENOCOP_TRUE 1
#define GENOCOP_FALSE 0

#define GENOCOP_SINGLE 0
#define GENOCOP_MULTIPLE 1

#define GENOCOP_MIN 1
#define GENOCOP_MAX 0

#define GENOCOP_RANDOM 0
#define GENOCOP_ORDERED 1
#define GENOCOP_DETERMINISTIC 1

#define GENOCOP_FIXED 0
#define GENOCOP_ADAPTIVE 1

#define GENOCOP_DOWN 0
#define GENOCOP_UP 1
#define	GENOCOP_MAXNUM 100.0F
#define GENOCOP_MINNUM -100.0F

#define GENOCOP_NTRIES 1000
#define GENOCOP_MAXCHARS 255
#define GENOCOP_MAXVECTOR 255

#define GENOCOP_NUMPARENTS 3

#define GENOCOP_U_FREQ_LIM 12.0
#define GENOCOP_L_FREQ_LIM 4.0
#define GENOCOP_RQFACTOR 0.1F	/* q factor for cum. prob. dist */
#define GENOCOP_SQFACTOR 0.1F	/* q factor for search population */

#define GENOCOP_MOVETRIES	100

#define GENOCOP_EPSILON 0.00001F
#define GENOCOP_INCREMENT 0.1

typedef float **MATRIX;
typedef float *VECTOR;
typedef int   **IMATRIX;
typedef int   *IVECTOR;


typedef struct
{
	int             r;
	int             c;
}               GENOCOP_INDEX;


namespace EvolutionaryOptimization
{
	struct Solver;
}

typedef struct
{
	int	iNumVars;		/* total no. of variables */
	int	iNumNLE;		/* no. of NL equalities */
	int	iNumNLIE;		/* no. of NL inequalities */
	int	iNumLC;			/* no. of linear constraints */
	int	iNumDC;			/* no. of domain constraints */
	int	iRefPopSize;	/* size of ref. pop. */
	int	iSearchPopSize;	/* size of search pop. */
	int	iNumOperators;	/* no. of operators */
	int	iRefPeriod;		/* period of evolution of ref. pop */
	int	iRefOffspring;	/* number of offspring for ref. pop. */
	int	iSelRefPoint;	/* GENOCOP_RANDOM or GENOCOP_ORDERED */
	int	iRepairMethod;  /* GENOCOP_RANDOM or DETERMINISTIC */
	int	iRefInitType;	/* SINGLE or MULTIPLE */
	int	iSearchInitType;/* SINGLE or MULTIPLE */
	int	iObjFnType;		/* GENOCOP_MAX or GENOCOP_MIN */
	int	iNumTestCase;	/* test case no. */
	int iSeed1;			/* seed no. 1 */
	int iSeed2;			/* seed no. 2 */
	int iFreqMode;		/* frequency mode- GENOCOP_FIXED or GENOCOP_ADAPTIVE */
	float	fReplRatio;	/* prob. of replacement for search pop. */
	float	fEpsilon;	/* EPSILON value for precision comparisons */
	long	lTotEvals;	/* total no. of evaluations */
	EvolutionaryOptimization::Solver *solver;
} EVOLDATA;





/*
**
** FUNCTION DECLARATIONS
**
**
*/


/*****************************************************************/
/* error handler - in ERRORS.C                                   */
/*****************************************************************/

void            error_exit(char error_text []);

/*****************************************************************/
/* memory allocation routines for arrays - in NUMERICS.C         */
/*****************************************************************/

VECTOR          init_vector(int ncols);
MATRIX          init_matrix(int nrows, int ncols);


/*****************************************************************/
/* memory deallocation routines  - in NUMERICS.C                 */
/*****************************************************************/

void            free_vector(VECTOR v);
void            free_matrix(float **m, int nrows);


/*****************************************************************/
/* random number generation/support routines - in GENOCOP_RANDOM.C       */
/*****************************************************************/

float randgen(float, float);
float random01(void);

/*****************************************************************/
/* static void randgen_init(int, int);                           */
/*****************************************************************/

void start_randgen(int iSeed1, int iSeed2);


/*****************************************************************/
/* file input/output routines - FILEIO.C                         */
/*****************************************************************/

void strip_comments(char *);


void	read_static_input(EVOLDATA *, FILE *);
void	write_static_output(EVOLDATA, FILE *);

void	read_DC(EVOLDATA, FILE *, MATRIX);
void    read_LC(EVOLDATA, FILE *, MATRIX);
void    read_OD(EVOLDATA, FILE *, VECTOR);

void	write_DC(EVOLDATA, FILE *, MATRIX);
void    write_LC(EVOLDATA, FILE *, MATRIX);
void    write_OD(EVOLDATA, FILE *, VECTOR);

void write_vector(VECTOR fV, FILE* fp, int iLength);


/*****************************************************************/
/* initialization routines - INIT.C                              */
/*****************************************************************/

void	assign_probab(VECTOR, int, float);
void reassign_probab(VECTOR, int, int, int);
void	init_ref_pop(EVOLDATA, MATRIX, MATRIX, MATRIX);
void	init_search_pop(EVOLDATA, MATRIX, MATRIX, MATRIX);
void	setup_rpop(EVOLDATA, MATRIX);
void	setup_spop(EVOLDATA, MATRIX, VECTOR, MATRIX, MATRIX, MATRIX);



void	ask_user(VECTOR, int);


/*****************************************************************/
/* miscellaneous utilities - UTILS.C                             */
/*****************************************************************/

float	power(float, int);
void print_vector(int, VECTOR);


/*****************************************************************/
/* optimization routines - OPTIMIZE.C                            */
/*****************************************************************/

void optimize(EVOLDATA, MATRIX, MATRIX, MATRIX, VECTOR, MATRIX,
	VECTOR, VECTOR, FILE *, VECTOR);


/*****************************************************************/
/* boundary/constraint check routines - VALIDATE.C               */
/*****************************************************************/

int check_LC(EVOLDATA, VECTOR, MATRIX, MATRIX);
int check_all(EVOLDATA, VECTOR, MATRIX, MATRIX);


/*****************************************************************/
/* operator routines - OPERATE.C                                 */
/*****************************************************************/

void normalize_opdist(VECTOR, int);
void cum_opdist(VECTOR, int);

int select_operator(VECTOR);
int operate(EVOLDATA, MATRIX, VECTOR, int, MATRIX, MATRIX, long, VECTOR,
	int);



/*****************************************************************/
/* operators - OPERATOR.C                                        */
/*****************************************************************/

void u_mutate(EVOLDATA, VECTOR, int, MATRIX, MATRIX);
void b_mutate(EVOLDATA, VECTOR, int, MATRIX, MATRIX);
void nu_mutate(EVOLDATA, VECTOR, int, MATRIX, MATRIX, long, int);
void hr_xover(EVOLDATA, VECTOR, VECTOR, VECTOR);
void gauss_mutate(EVOLDATA, VECTOR, long);
int pool_xover(EVOLDATA, int, VECTOR, VECTOR, MATRIX, int);
int scatter_xover(EVOLDATA, int, VECTOR, VECTOR, MATRIX, int);


/*****************************************************************/
/* sort/replace/reorder routines - SORT.C                        */
/*****************************************************************/

void sort(MATRIX, int, int);
void swap(float **, float **);
void insert_vector(MATRIX, VECTOR, int, int, int);

void replace_r_vector(EVOLDATA, MATRIX, int, VECTOR);
void replace_s_vector(EVOLDATA, MATRIX, int, VECTOR);



/*****************************************************************/
/* evaluation routines - EVALUATE.C                              */
/*****************************************************************/

void	eval_ref_pop(MATRIX);
void	eval_search_pop(MATRIX, MATRIX);
void	eval_ref_vector(EVOLDATA, VECTOR);
void	eval_search_vector(EVOLDATA, int, VECTOR,
	MATRIX, MATRIX, MATRIX, MATRIX);



/*****************************************************************/
/* function routines - FUNCTION.C                                */
/*****************************************************************/

void eval_func(VECTOR, int, EVOLDATA);



/********************** End of GENOCOP.H header file *************/
