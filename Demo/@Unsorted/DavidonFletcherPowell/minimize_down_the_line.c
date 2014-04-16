

#include <math.h>

#define DBL_EPSILON 0.00001

struct arg_list 
{ 
	double* x; 
	double* v; 
	double(*f)(double*); 
	double *y; 
	int n; 
}; 

static struct arg_list *args = 0;

int Min_Search_Parabolic_Extrapolation(double(*f)(double), double x [], double fx [], double a, double b, double cut_off_scale_factor);
int Min_Search_Parabolic_Interpolation(double(*f)(double), double x [], double fx [], double tolerance);
	
void Ray_in_n_Space(double v[], double initial_pt[], double direction[], double distance, int n ) 
{ 
	for (--n; n >= 0; n--) v[n] = initial_pt[n] + distance * direction[n];
} 

static double g(double lambda);

int Minimize_Down_the_Line(
	double(*f)(double *), 
	double x [], 
	double fx, 
	double *p, 
	double v [], 
	double y [], 
	double cutoff, 
	double cutoff_scale_factor, 
	double tolerance, 
	int n
	)
{
	struct arg_list g_args;
	struct arg_list *old_args;
	double lambda[3];
	double g_lambda[3];
	int k = 0;
	g_args.x = x;
	g_args.v = v;
	g_args.f = f;
	g_args.y = y;
	g_args.n = n;

	// Bracket the minimum 

	lambda[0] = 0.0;
	lambda[2] = 1.0;
	old_args = args;
	args = &g_args;
	g_lambda[0] = fx;
	g_lambda[2] = g(lambda[2]);
	*p = (g_lambda[0] < g_lambda[2]) ? 0.0 : 1.0;
	args = &g_args;
	k = Min_Search_Parabolic_Extrapolation(g, lambda, g_lambda, 0.0, cutoff, cutoff_scale_factor);

	if (k < 0) k = 0; else
	{
		// Interpolate for the minimum 
		// The minimum is located between lambda[0] < lambda[1] < lambda[2], 
		// where g(lambda[1]) < g(lambda[0]) and g(lambda[1]) < g(lambda[2]), 
		// i.e. f(x - lambda[0]*v) > f(x - lambda[1]*v) < f(x - lambda[2]*v). 

		if (tolerance <= 0.0)
			tolerance = sqrt(DBL_EPSILON);
		args = &g_args;

		k = Min_Search_Parabolic_Interpolation(g, lambda, g_lambda, tolerance);
		*p = 0.5 * (lambda[0] + lambda[2]);
	}

	Ray_in_n_Space(y, x, v, -(*p), n);
	args = old_args;
	return k;
}


static double g(double lambda) 
{ 
	Ray_in_n_Space(args->y, args->x, args->v, -lambda, args->n); 
	return (args->f)(args->y);
}