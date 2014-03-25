#include <Windows.h>
#include <iostream>

static char s_msg[1024];

extern "C"
{ 
	int Fletcher_Powell_Davidon(
	double(*f)(double *),
	void(*df)(double*, double*),
	int(*Stopping_Rule)(double*, double*, int, int),
	double *a,
	double *dfa,
	double line_search_cutoff,
	double line_search_cutoff_scale_factor,
	double line_search_tolerance,
	int n
	);
}

double f(double *x)
{
	double x1, x2, result;

	x1 = x[0];
	x2 = x[1];

	result =
		(x1 - 1) * x1 + 
		x2 * (x2 + 2);
		/*+ 12 * x1 * x1 
		- 1 * x1 * x2 
		+ 3 * x2 * x2 
		- 2 * x1 
		+ 10 * x2;*/

	sprintf_s(s_msg, "f:   x1=%2.6f x2=%2.6f => %2.6f\n", x1, x2, result);
	OutputDebugStringA(s_msg);

	return result;
}

void df(double *x, double *dx)
{
	/*double dfdx1, dfdx2;
	double fx1, fx2;

	fx1 = x[0];
	fx2 = x[1];

	dfdx1 = 24 * fx1 - fx2 - 2;
	dfdx2 = -fx1 + 6 * fx2 + 10;

	sprintf_s(s_msg, "x1=%2.2f x2=%2.2f dx1=%2.2f dx2=%2.2f\n", 
		fx1, fx2, dfdx1, dfdx2
		); OutputDebugStringA(s_msg);

	dx[0] = dfdx1;
	dx[1] = dfdx2;*/

	double x1 = x[0];
	double x2 = x[1];
	dx[0] = 2 * x1 - 1;
	dx[1] = 2 * x2 + 2;

	sprintf_s(s_msg, "df:   x1=%2.6f dx1=%2.6f \n",
		x[0], dx[0]
		); OutputDebugStringA(s_msg);
}

int stop(double * v, double *dx, int i, int n)
{
	sprintf_s(s_msg, "stop: v1=%2.6f v2=%2.6f fv1=%2.6f dx2=%2.6f\n",
		v[0], v[1], dx[0], dx[1]
		); OutputDebugStringA(s_msg);

	if (abs(dx[0]) <= 0.00001) return true;
	if (abs(dx[1]) <= 0.00001) return true;

	if (abs(v[0]) <= 0.00001) return true;
	if (abs(v[1]) <= 0.00001) return true;
	if (i > 500) return true;

	return false;
}


int main(int argc, char **argv)
{
	//int result = 

	double fx[2];
	double dfdx[2];

	fx[0] = -12.5;
	fx[1] = 12.5;

	double cutoff = 0.0001; // 10.1;
	double cutoff_scale = 0.00001; // 1.0;
	double tolerance = 0.00001;
	int n = 2;
	//int n = 2;


	Fletcher_Powell_Davidon(
		&f, &df, &stop, 
		fx, dfdx,
		cutoff, 
		cutoff_scale, 
		tolerance, 
		n
		);

	double x1, x2;
	x1 = fx[0];
	x2 = fx[1];

	double result = f(fx);

	sprintf_s(s_msg, "min:   x1=%2.6f x2=%2.6f => %2.6f\n", x1, x2, result);
	OutputDebugStringA(s_msg);

	return 0;
}