#include <Windows.h>
#include <debugapi.h>
#include <math.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

#define _Dbg_decl static char g_dbg_msg[1024];
#define _Write_ln(ln) { OutputDebugStringA(ln);  OutputDebugStringA("\n"); }
#define _Write(ln) { OutputDebugStringA(ln); }
#define _Next_ln { OutputDebugStringA("\n"); }
#define _Write_dbg_ln _Write_ln(g_dbg_msg)
#define _Write_dbg _Write(g_dbg_msg)
#define _Assert_wmsg _ASSERT_EXPR // Assert with message
#define _Sprintf_begin sprintf_s(g_dbg_msg,
#define _Sprintf_end ); _Write_dbg;
#define _Sprintf_end_ln ); _Write_dbg_ln;
#define _Perf_decl LARGE_INTEGER g_StartCounter, g_EndCounter, g_DeltaCounter;
#define _Perf_begin QueryPerformanceCounter(&g_StartCounter);
#define _Perf_end QueryPerformanceCounter(&g_EndCounter); \
	g_DeltaCounter.HighPart = g_EndCounter.HighPart - g_StartCounter.HighPart; \
	g_DeltaCounter.LowPart = g_EndCounter.LowPart - g_StartCounter.LowPart;

#define _Perf_print _Sprintf_begin "Performance = %u/%u ",	g_DeltaCounter.HighPart, g_DeltaCounter.LowPart _Sprintf_end;

_Dbg_decl;

struct Problem
{
public:

	typedef std::vector<uint32_t> IntegerVector;
	typedef IntegerVector::value_type Integer;
	typedef IntegerVector::iterator IntegerVectorIterator;
	
	Integer Number;
	IntegerVector Factors;

	void Factorize() 
	{
		Factors.clear();

		Integer n = Number;
		for (Integer f = 2; f <= Number; f++)
		{
			auto r = n / f;
			auto d = n % f;

			if (d == 0) 
			{
				Factors.push_back(f);
				while (d == 0 && n != 0)
				{
					n = r;
					r = n / f;
					d = n % f;

					if (d == 0) 
					{
						Factors.push_back(f);
					}
				}
			}
		}
	}
};

void InitializeProblem( _In_ Problem *problem )
{
	problem->Number = 24;
};

_Perf_decl;

void main()
{
	Problem problem;
	InitializeProblem(&problem);

	_Perf_begin problem.Factorize(); _Perf_end
	_Perf_print _Next_ln

	_Sprintf_begin "%d = ", problem.Number _Sprintf_end
	if ( problem.Factors.size() > 1 )
	{
		_Sprintf_begin "%d", problem.Factors[0] _Sprintf_end
		for ( auto i = 1; i < problem.Factors.size(); i++)
		{
			_Sprintf_begin "*%d", problem.Factors[i] _Sprintf_end
		}

		_Sprintf_begin "." _Sprintf_end_ln
	}
	else 
	{
		_Write_ln("prime.")
	}
};