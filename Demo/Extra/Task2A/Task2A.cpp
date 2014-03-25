#include <Windows.h>
#include <debugapi.h>
#include <math.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
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

#define _Sprintf_begin sprintf_s(g_dbg_msg,
#define _Sprintf_end ); _Write_dbg;
#define _Sprintf_end_ln ); _Write_dbg_ln;


class Problem
{
public:

	typedef std::vector<uint32_t> IntegerVector;
	typedef IntegerVector::value_type Integer;
	typedef IntegerVector::iterator IntegerVectorIterator;
	static Integer const LeftBoundary = (Problem::Integer)2;
	static Integer const RightBoundary = (Problem::Integer)1000000;

	inline static void PrecalculatePrimeIntegers(
		_In_ Problem::Integer const leftBoundary = LeftBoundary,
		_In_ Problem::Integer const rightBoundary = RightBoundary
		)
	{
		Problem::Integer const rightBoundarySqrt = (Problem::Integer)(sqrt(RightBoundary)); 
		for (Problem::Integer i = leftBoundary; i <= rightBoundarySqrt; i++)
		{
			if (IsPrimeInteger(i)) s_primes.push_back(i);
		}
	}
	
private:

	inline static bool IsPrimeInteger(
		_In_ Problem::Integer const i
		)
	{
		Problem::Integer const s = (Problem::Integer)(sqrt(i)); 
		for (Problem::Integer j = 2; j <= s; j++) if ((i % j) == 0) return false;
		return true;
	}

	inline static bool IsPrimeIntegerOpt(
		_In_ Problem::Integer const i
		)
	{
		Problem::Integer const s = (Problem::Integer)(sqrt(i)); 
		for (Problem::Integer prime : s_primes) if ((prime < s) && ((i % prime) == 0)) return false;
		return true;
	}	

	static IntegerVector s_primes;

public:

	Integer From, To;
	IntegerVector Primes;

	void PrintPrimes()
	{
		Primes.clear();

		for (Problem::Integer i = From; i <= To; i++)
		{
			if (IsPrimeIntegerOpt(i)) 
			{
				Primes.push_back(i); //_Sprintf_begin "%d is prime\n", i _Sprintf_end; 
			}
		}
	}
};

Problem::IntegerVector Problem::s_primes;

void Setup()
{
	Problem::PrecalculatePrimeIntegers();
}

void InitializeProblem( _In_ Problem *problem )
{
	problem->From = 2;
	problem->To = 300;
}

void main()
{
	Setup();

	Problem problem;
	InitializeProblem(&problem);

	LARGE_INTEGER start; QueryPerformanceCounter(&start);
	problem.PrintPrimes();
	LARGE_INTEGER end; QueryPerformanceCounter(&end);

	_Sprintf_begin "Performance = %u ",	end.LowPart - start.LowPart _Sprintf_end
	_Sprintf_begin "Range=[%d; %d] Size=%d\n", problem.From, problem.To, problem.Primes.size() _Sprintf_end
	for ( auto i : problem.Primes )	_Sprintf_begin "\t%d\n", i _Sprintf_end
}