#include <Windows.h>
#include <debugapi.h>
#include <math.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
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
	typedef std::vector<std::string> StringVector;
	static const int16_t MaxLength = 16i16; //8i16; // Max string length allowed.
	static const int16_t MinLength = 2i16; // Min string length allowed.
	typedef char String[MaxLength]; 

	inline void SetExpression( _In_ String e, _In_ int16_t length ) 
	{ 
		_Assert_wmsg(length <= MaxLength, L"Problem::MaxLength"); 
		_Assert_wmsg(length >= MinLength, L"Problem::MinLength"); 
		memcpy(InitialExpression, e, MaxLength); 
		Length = length; 
	} 

	inline String const *GetExpression( ) 
	{
		return &InitialExpression;
	}

	inline void GetExpression( _Inout_ String expression ) 
	{
		memcpy(expression, InitialExpression, MaxLength); 
	}

	inline void Permut( ) 
	{ 
		Permutations.clear(); 
		Permut(0, InitialExpression);
	}

	StringVector Permutations; // This collection contains all possible premutations of the initial expression

private:

	String InitialExpression; // Initial expression
	int16_t Length; // Expression length

	inline void Swap( _Inout_ String expression, _In_ int16_t i, _In_ int16_t j ) 
	{ 
		std::swap(expression[i], expression[j]);
	}

	void Permut( _In_ int16_t s, _In_ String prev, uint32_t depth = 1 )
	{
		if (s == Length) return; else 
		{
			String current = { 0i8 }; // memcpy(current, prev, MaxLength);

			//Permutations.push_back(prev);
			for (int16_t i = s + 1; i < Length; i++)
			{
				memcpy(current, prev, MaxLength);
				Swap(current, s, i); //_Write_ln(current); 
				Permutations.push_back(current);

				/*_Sprintf_begin "Adding %s (%d) \n", 
					current, depth 
					_Sprintf_end*/

				Permut(s + 1, current, depth + 1);
			}
		}
	}
};

void InitializeProblem( _In_ Problem *problem )
{
	// Test case 1
	//Problem::String expression = { 'S', 'E', 'R', 'H', 'I', 'I', 'E', 'K', 'O' };
	//problem->SetExpression(expression, 9);

	// Test case 2
	Problem::String expression = { 'X', 'I', 'O', 'E' };
	problem->SetExpression(expression, 4);

}

_Perf_decl;

void main()
{
	Problem problem;
	InitializeProblem(&problem);

	_Perf_begin problem.Permut(); _Perf_end
	_Perf_print _Next_ln

	_Sprintf_begin "Expression permutations(%d):", problem.Permutations.size() _Sprintf_end_ln
	for ( auto e : problem.Permutations ) { _Write("\t >> ") _Write_ln(e.c_str()) }
}