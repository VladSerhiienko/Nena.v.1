
#include "genocop.h"

static void randgen_init(int iSeed1, int iSeed2);

static float u[98], c, cd, cm;
static int i97, j97;
static int test = GENOCOP_FALSE;


float randgen(float fLlim, float fUlim)
{
	float fRandomVal;
	
	fRandomVal = random01();
	
	return(	fLlim + (float) (fRandomVal * (fUlim - fLlim)) );	
	
}
/**************************************************************************/




/*
* This is the initialization routine for the random number generator.
* NOTE: The seed variables can have values between:    0 <= iSeed1 <= 31328
*                                                      0 <= iSeed2 <= 30081
* The random number sequences created by these two seeds are of sufficient 
* length to complete an entire calculation with. For example, if sveral 
* different groups are working on different parts of the same calculation,
* each group could be assigned its own iSeed1 seed. This would leave each group
* with 30000 choices for the second seed. That is to say, this random 
* number generator can create 900 million different subsequences -- with 
* each subsequence having a length of approximately 10^30.
* 
* Use iSeed1 = 1802 & iSeed2 = 9373 to test the random number generator. The
* subroutine RANMAR should be used to generate 20000 random numbers.
* Then display the next six random numbers generated multiplied by 4096*4096
* If the random number generator is working properly, the random numbers
* should be:
*           6533892.0  14220222.0  7275067.0
*           6172232.0  8354498.0   10633180.0
*/
static void randgen_init(int iSeed1, int iSeed2)
{

	int i, j, k, l, ii, jj, m;
	float s, t;
	
	if (iSeed1<0 || iSeed1>31328 || iSeed2<0 || iSeed2>30081) 
		error_exit("Seed value out of Range. function: randgen_init()"); 
	
	i = (iSeed1/177)%177 + 2;
	j = iSeed1%177 + 2;
	k = (iSeed2/169)%178 + 1;
	l = iSeed2%169;
	
	for (ii=1; ii<=97; ii++) {
		s = 0.0;
		t = 0.5;
		for (jj=1; jj<=24; jj++) {
			m = (((i*j)%179)*k) % 179;
			i = j;
			j = k;
			k = m;
			l = (53*l + 1) % 169;
			if ((l*m)%64 >= 32) s += t;
			t *= 0.5;
		}
		u[ii] = s;
	}
	
	c = 362436.0 / 16777216.0;
	cd = 7654321.0 / 16777216.0;
	cm = 16777213.0 / 16777216.0;
	
	i97 = 97;
	j97 = 33;
	
	test = GENOCOP_TRUE;

}
/**************************************************************************/



/*
**
** float random01(void)
**
** returns a random number between 0 and 1
**
*/
float random01(void)
{
	int ivec;
	float uni;
	
	if (test == GENOCOP_FALSE)
	{
		puts("Call the init routine rmarin() before calling ranmar().");
		exit(2);
	}

	uni = u[i97] - u[j97];
	if (uni < 0.0) uni += 1.0;
	u[i97] = uni;
	i97--;
	if (i97==0) i97 = 97;
	j97--;
	if (j97==0) j97 = 97;
	c -= cd;
	if (c<0.0) c += cm;
	uni -= c;
	if (uni<0.0) uni += 1.0;
		
	return(uni);
}
/**************************************************************************/


/*
**
** void start_randgen(int iSeed1, int iSeed2)
**
** Seeds the random number generator
**
*/
void start_randgen(int iSeed1, int iSeed2)
{
	randgen_init(iSeed1, iSeed2); 
}
/**************************************************************************/
