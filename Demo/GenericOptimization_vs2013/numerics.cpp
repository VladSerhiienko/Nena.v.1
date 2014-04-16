
#include <stdio.h>
#include "genocop.h"


/*
**
** VECTOR init_vector(int ncols)
**
** Allocates enough memory for an array of floats from 0 to ncols - 1,
**
** and returns a VECTOR i.e float * pointer.
**
**
*/
VECTOR
init_vector(int ncols)
{
    VECTOR          v;

    if (ncols <= 0)
	return (NULL);

    v = (float *) malloc((unsigned) (ncols) * sizeof(float));
    if (!v)
	error_exit("Memory allocation failure. function: init_vector()");
    return (v);
}
/**************************************************************************/




/*
**
** MATRIX init_matrix(int nrows, int ncols)
**
** Allocates enough memory for a matrix array of floats,
**
** with nrows and ncols
**
** and returns an MATRIX i.e float ** pointer.
**
**
*/
MATRIX
init_matrix(int nrows, int ncols)
{
    int             i;
    MATRIX          m;

    if (nrows <= 0)
	return (NULL);
    if (ncols <= 0)
	return (NULL);

    m = (float **) malloc((unsigned) (nrows) * sizeof(float **));
    if (!m)
	error_exit("Memory allocation failure. function: init_matrix()");

    for (i = 0; i < nrows; i++)
    {
	m[i] = (float *) malloc((unsigned) (ncols) * sizeof(float));
	if (!m[i])
	    error_exit("Memory allocation failure. function: init_matrix()");
    }
    return m;
}
/**************************************************************************/



/*
** void free_vector(VECTOR v)
**
** Frees the memory allocated for VECTOR v with n elements.
**
**
**
**
*/
void
free_vector(VECTOR v)
{
    if (v == NULL)
	return;
    else
	free((float *) (v));
}
/**************************************************************************/


/*
** void free_matrix (MATRIX m, int nrows)
**
** Frees the memory allocated for MATRIX m.
**
**
**
**
*/
void
free_matrix(MATRIX m, int nrows)
{
    int             i;

    if (m == NULL)
	return;
    else
    {
	for (i = 0; i < nrows; i++)
	    free((float *) (m[i]));
	free((float **) (m));
    }
}
/**************************************************************************/


