#include <Windows.h>
#include <debugapi.h>
#include <math.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
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

struct Problem
{
	long from;
	long to;

	Problem() : from(0), to(0) { }

	__forceinline bool IsPrime(long const i)
	{
		long const s = (long)sqrt(i); for (long j = 2; j <= s; j++) 
		{
			bool const zero = (i % j) == 0; // the divisor is found
			if (zero) return false; // is not prime
		}

		return true; // no divisors found - is prime
	}

	void PrintPrimes()
	{
		for (long i = from; i <= to; i++)
		{
			if (IsPrime(i)) { sprintf_s(g_dbg_msg, "%d\n", i); _Write_dbg; }
			//else { sprintf_s(g_dbg_msg, "%d is not prime\n"); _Write_dbg; }
		}
	}
};

void InitializeProblem(Problem *problem)
{
	ZeroMemory(g_dbg_msg, sizeof g_dbg_msg);

	problem->from = 2;
	problem->to = 1000;

	// Result: 
	// Primes in the output window
}

void main()
{
	Problem problem;
	InitializeProblem(&problem);
	problem.PrintPrimes();
}
