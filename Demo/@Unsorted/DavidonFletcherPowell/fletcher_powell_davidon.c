
#include <math.h>
#define NULL 0

#define DBL_EPSILON 0.00001

void Subtract_Vectors(double w[], double u[], double v[], int n) { register int i; for (i = 0; i < n; i++) w[i] = u[i] - v[i]; }
void Copy_Vector(double *d, double *s, int n) { memcpy(d, s, sizeof(double) * n); }
double Inner_Product(double u[], double v[], int n) { double inner_product = 0.0; for (n--; n >= 0; n--) inner_product += u[n] * v[n]; return inner_product; }
void Identity_Matrix_ut(double *A, int n) { int i, j; for (i = 0; i < n;) { *A++ = 1.0; for (j = ++i; j < n; j++) *A++ = 0.0; } }

int Minimize_Down_the_Line(double(*f)(double *), double x[], double fx, double *p, double v[], double y[], double cutoff, double cutoff_scale_factor, double tolerance, int n); 
void Multiply_Sym_Matrix_by_Vector_ut(double *u, double *A, double* v, int n); 

void Multiply_Sym_Matrix_by_Vector_ut(double *u, double *A, double* v, int n)
{
	double *pA; double *qA; int i, j; 
	for (pA = A, i = 0; i < n; i++)
	{ 
		for (j = i, u[i] = 0.0; j < n; j++) u[i] += (*pA++) * v[j]; 
		for (j = 0, qA = A + i; j < i; j++, qA += n - j) u[i] += *qA * v[j]; 
	} 
} 

void Update_H(double *H, double* dx, double* dg, double* Hg, int n);

int Fletcher_Powell_Davidon(
	double(*f)(double *), 
	void(*df)(double*, double*), 
	int(*Stopping_Rule)
	(double*, double*, int, int), 
	double *a, 
	double *dfa, 
	double line_search_cutoff, 
	double line_search_cutoff_scale_factor, 
	double line_search_tolerance, int n
	) 
{ 
	double *x; 
	double *dx;
	double *dg; 
	double *H; 
	double *Hg;
	double *v; 
	double p; 
	int iteration = 0;
	int count_between_resets = n; 
	int err = 0; 
	x = (double *) malloc(n * sizeof(double)); 
	v = (double *) malloc(n * sizeof(double)); 
	dx = (double *) malloc(n * sizeof(double)); 
	dg = (double *) malloc(n * sizeof(double)); 
	Hg = (double *) malloc(n * sizeof(double)); 
	H = (double *) malloc(((n * (n + 1)) >> 1) * sizeof(double)); 
	if (H == NULL) 
		err = -1; 
	else 
	{ 
		df(a, dfa); 
		do 
		{ 
			iteration++; count_between_resets++; 
			if (count_between_resets > n) 
			{ 
				Identity_Matrix_ut(H, n); 
				count_between_resets = 1; 
			} 
		
			Multiply_Sym_Matrix_by_Vector_ut(v, H, dfa, n); 
			Minimize_Down_the_Line(f, a, f(a), &p, v, x, line_search_cutoff, line_search_cutoff_scale_factor, line_search_tolerance, n); 
			Subtract_Vectors(dx, x, a, n); 
			Copy_Vector(a, x, n); 
			Copy_Vector(x, dfa, n); 
			df(a, dfa); 
			Subtract_Vectors(dg, dfa, x, n); 
			Update_H(H, dx, dg, Hg, n); 
			err = Stopping_Rule(dx, dfa, iteration, n); 
		} 
		while (!err); 
	} 

	free(Hg); 
	free(H); 
	free(dg); 
	free(dx); 
	free(v); 
	free(x); 
	return err; 
} 

static void Update_H(double *H, double* dx, double* dg, double* Hg, int n) 
{ 
	double *Hdg; double dxtdg; double dgtHdg; double *pH; int i, j; 

	dxtdg = Inner_Product(dx, dg, n); 
	Multiply_Sym_Matrix_by_Vector_ut(Hg, H, dg, n); 
	dgtHdg = Inner_Product(dg, Hg, n); 
	dxtdg = 1.0 / dxtdg; 
	dgtHdg = 1.0 / dgtHdg; 
	pH = H; 

	for (i = 0; i < n; i++) 
	for (j = i; j < n; j++) 
		*pH++ += dxtdg * dx[i] * dx[j] - dgtHdg * Hg[i] * Hg[j]; 
}