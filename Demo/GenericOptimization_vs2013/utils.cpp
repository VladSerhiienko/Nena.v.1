
#include <stdio.h>
#include "genocop.h"


/*
**
** float power(float x, int y)
**
** Raises x to the power y and returns the result.
**
**
*/
float
power(float x, int y)
{
    int             i;
    float           tot = 1.0F;

    for (i = 0; i < y; i++)
		tot = tot * x;
    return (tot);
}
/**************************************************************************/


/*
**
** void print_vector(int iLength, VECTOR fV)
**
** Raises x to the power y and returns the result.
**
**
*/
void print_vector(int iLength, VECTOR fV)
{
	int i;
	
	printf("--Print Vector---------------------------------\n");
	for (i=1; i<=iLength; i++)
		printf("%f   ", fV[i]);
	printf("\n");
	printf("-----------------------------------------------\n");


}
/**************************************************************************/
	
