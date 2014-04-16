
#define SQRT5 2.236067977499789696 
static const double alpha = 0.5 * (3.0 - SQRT5);
static double min_displacement;

#define DBL_EPSILON 0.001

static double New_Test_Point(double x [], double y [], double a, double b, double cut_off_scale_factor);
static double Parabolic_Minimum(double x[], double y[], double cut_off_scale_factor);

int Min_Search_Parabolic_Extrapolation(double(*f)(double), double x [], double fx [], double a, double b, double cut_off_scale_factor)
{
	double xmin, ymin; int return_code = 0; min_displacement = sqrt(DBL_EPSILON);
	// Verify that x[0] < x[2] 

	if (x[0] == x[2]) return -2;
	if (x[0] > x[2])
	{
		x[1] = x[0]; fx[1] = fx[0]; x[0] = x[2]; fx[0] = fx[2]; x[2] = x[1]; fx[2] = fx[1];
	}
	// Set x[1] such that x[0] < x[1] < x[2] 

	if (fx[0] < fx[2])
		x[1] = x[0] + alpha * (x[2] - x[0]); x[1] = x[2] - alpha * (x[2] - x[0]); fx[1] = f(x[1]);
	// Iterate until either f(x[0]) > f(x[1]) < f(x[2]) 
	// or x[0] == a or x[2] == b. 

	while ((fx[0] <= fx[1]) || (fx[2] <= fx[1]))
	{
		xmin = New_Test_Point(x, fx, a, b, cut_off_scale_factor);
		if ((xmin == a) || (xmin == b)) { return_code = -1; break; }
		ymin = f(xmin); if (xmin < x[0])
		{
			x[2] = x[1]; fx[2] = fx[1]; x[1] = x[0]; fx[1] = fx[0]; x[0] = xmin; fx[0] = ymin;
		}
		else if (xmin > x[2]) { x[0] = x[1]; fx[0] = fx[1]; x[1] = x[2]; fx[1] = fx[2]; x[2] = xmin; fx[2] = ymin; }
		else if (xmin < x[1]) if ((ymin >= fx[0]) && (ymin <= fx[1]))
		{
			x[2] = x[1]; fx[2] = fx[1]; x[1] = xmin;
			fx[1] = ymin;
		}
		else if (ymin > fx[1]) { x[0] = xmin; fx[0] = ymin; break; }
		else { x[2] = x[1]; fx[2] = fx[1]; x[1] = xmin; fx[1] = ymin; break; }
		else if ((ymin >= fx[2]) && (ymin <= fx[1])) { x[0] = x[1]; fx[0] = fx[1]; x[1] = xmin; fx[1] = ymin; }
		else if (ymin > fx[1]) { x[2] = xmin; fx[2] = ymin; break; }
		else if (ymin < fx[2])
		{
			x[1] = xmin; fx[1] = ymin;
			break;
		}
	}

	return return_code;
}

static double New_Test_Point(double x [], double y [], double a, double b, double cut_off_scale_factor)
{
	double xmin; double eta; xmin = Parabolic_Minimum(x, y, cut_off_scale_factor);
	// Compare the location of the minimum of the 
	// parabolic fit to the left-most node. 

	eta = (x[0] == 0.0) ? 1.0 : fabs(x[0]);
	if (fabs(xmin - x[0]) < (min_displacement * eta)) xmin -= 100.0 * min_displacement * eta;
	else
	{
		// Compare the location of the minimum of the 
		// parabolic fit to the center node. 
		eta = (x[1] == 0.0) ? 1.0 : fabs(x[1]);
		if (fabs(xmin - x[1]) < (min_displacement * eta))
		if (y[2] < y[0]) xmin = x[1] + alpha * (x[2] - x[1]);
		else xmin = x[1] - alpha * (x[1] - x[0]); else
		{
			// Compare the location of the minimum of the 
			// parabolic fit to the right-most node. 

			eta = (x[2] == 0.0) ? 1.0 : fabs(x[2]);
			if (fabs(x[2] - xmin) < (min_displacement * eta))
				xmin += 100.0 * min_displacement * eta;
		}
	}

	if (xmin < a) xmin = a;
	if (xmin > b) xmin = b;

	return xmin;
}

static double Parabolic_Minimum(double x [], double y [], double cut_off_scale_factor)
{
	double d1 = (x[2] - x[1]) * (y[1] - y[0]);
	double d2 = (x[1] - x[0]) * (y[2] - y[1]);
	double denominator = 2.0 * (d1 - d2);
	double numerator = (x[2] + x[1]) * d1 - (x[1] + x[0]) * d2;
	double x_lb = x[0] - cut_off_scale_factor * (x[1] - x[0]);
	double x_ub = x[2] + cut_off_scale_factor * (x[2] - x[1]);
	double ymax = (y[2] - y[0]) * (x[1] - x[0]) / (x[2] - x[0]) + y[0];
	double xmin; if (y[1] >= ymax) xmin = (y[0] < y[2]) ? x_lb : x_ub;
	else
	{
		if (x_lb * denominator <= numerator) xmin = x_lb;
		else if (x_ub * denominator >= numerator) xmin = x_ub;
		else xmin = numerator / denominator;
	}
	return xmin;
}