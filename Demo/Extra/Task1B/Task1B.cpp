#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <Windows.h>
#include <debugapi.h>
using namespace std;

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof x / x[0])
#endif

static char g_dbg_msg[1024];
#define _Write_ln(ln) { OutputDebugStringA(ln);  OutputDebugStringA("\n"); }
#define _Write(ln) { OutputDebugStringA(ln); }
#define _Next_ln { OutputDebugStringA("\n"); }
#define _Write_dbg_ln _Write_ln(g_dbg_msg)
#define _Write_dbg _Write(g_dbg_msg)

struct Problem // Task 1B
{
	const static int g_max = 24; // max number of terms in the expression
	char sign[g_max - 1]; // signes of the expression
	long x[g_max]; // numbers of the expression
	int n; // number of terms in the expression
	int s; // desired expression result

	bool solved;
	char minus;
	char plus;

	Problem() : n(0), s(0), minus('-'), plus('+'), solved(false)
	{
		memset(sign, 0, ARRAYSIZE(sign));
		memset(x, 0, ARRAYSIZE(x));
	}

	int Calc()
	{
		int sum = x[0]; for (int i = 1; i < n; i++)
		{
			sum += sign[i - 1] == minus ? -x[i] : x[i];
		} return sum;
	}

	void DbgPrintout()
	{
		std::stringstream ss;

		ss << x[0]; for (int i = 1; i < n; i++)
		{
			ss << sign[i - 1] << x[i];
		} 

		if (solved) ss << " = "; else ss << " ? ";
		ss << s << ".";
		{
			auto str = ss.str();
			_Write_ln(str.c_str());
		}
	}

	void Solve()
	{
		Solve(n, 0);
	}

private:

	void Solve(int k, long sum)
	{
		sprintf_s(g_dbg_msg, "k=%d sum=%d\t", k, sum); _Write_dbg;

		if (s == sum) 
		{ 
			solved = true; 
			DbgPrintout();
			return; 
		}

		DbgPrintout();

		if (k == 1)
		{
			if (s == sum) solved = true;
			else solved = false;
		}
		else if (k > 1)
		{
			sign[k - 1] = minus; Solve(k - 1, Calc()); if (solved) return;
			sign[k - 1] = plus; Solve(k - 1, Calc()); if (solved) return;
		}
	}
};

void InitializeProblem(Problem *problem)
{
	problem->n = 5;
	problem->s = 11;

	problem->x[0] = 5;
	problem->x[1] = 3;
	problem->x[2] = 1;
	problem->x[3] = 6;
	problem->x[4] = 2;

	problem->sign[0] = problem->minus;
	problem->sign[1] = problem->minus;
	problem->sign[2] = problem->minus;
	problem->sign[3] = problem->minus;

	// Result: 
	// 5-3+1+6+2=11. (faster)
	// Or:
	// 5+3-1+6-2=11.
}

void main()
{
	Problem problem;
	InitializeProblem(&problem);
	problem.Solve();

	if (problem.solved) { _Next_ln; _Write("Solution:\t"); problem.DbgPrintout(); _Next_ln; }
	else _Write_ln("No solution.");
}