

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define SINGLE 0
#define MULTIPLE 1



#define MIN 1
#define MAX 0


#define RANDOM 0
#define ORDERED 1
#define DETERMINISTIC 1

#define FIXED 0
#define ADAPTIVE 1

#define DOWN 0
#define UP 1
#define	MAXNUM 100.0F
#define MINNUM -100.0F


#define NTRIES 1000
#define MAXCHARS 255
#define MAXVECTOR 255


#define NUMPARENTS 3

#define U_FREQ_LIM 12.0
#define L_FREQ_LIM 4.0

#define MOVETRIES	100

#define EPSILON 0.00001F
#define INCREMENT 0.1

#define RQFACTOR 0.1F	/* q factor for cum. prob. dist */
#define SQFACTOR 0.1F	/* q factor for search population */

typedef float **MATRIX;
typedef float *VECTOR;
typedef int   **IMATRIX;
typedef int   *IVECTOR;


typedef struct
{
	int             r;
	int             c;
}               INDEX;


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
	int	iSelRefPoint;	/* RANDOM or ORDERED */
	int	iRepairMethod;  /* RANDOM or DETERMINISTIC */
	int	iRefInitType;	/* SINGLE or MULTIPLE */
	int	iSearchInitType;/* SINGLE or MULTIPLE */
	int	iObjFnType;		/* MAX or MIN */
	int	iNumTestCase;	/* test case no. */
	int iSeed1;			/* seed no. 1 */
	int iSeed2;			/* seed no. 2 */
	int iFreqMode;		/* frequency mode- FIXED or ADAPTIVE */
	float	fReplRatio;	/* prob. of replacement for search pop. */
	float	fEpsilon;	/* EPSILON value for precision comparisons */
	long	lTotEvals;	/* total no. of evaluations */
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
/* random number generation/support routines - in RANDOM.C       */
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
