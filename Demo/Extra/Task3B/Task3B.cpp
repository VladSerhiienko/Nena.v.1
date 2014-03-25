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

typedef char char8_t;
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

	typedef struct Item
	{
		char8_t s;
		uint32_t i;
		//uint32_t imax;

		void Print()
		{
			_Sprintf_begin "[%c%d]", s, i
				_Sprintf_end
		}
	} *Items;

	enum 
	{
		eItemSize = sizeof Item
	};

	Problem() : InitState(nullptr), Size(0)
	{
	}

	std::string GenerateString( Items item, uint32_t size )
	{
		std::string s; for (uint32_t i = 0; i < size; i++)
		{
			s += item[i].s;
		} return s;
	}

	void SetInitialState( std::string state )
	{
		if (InitState)
		{
			delete [] InitState;
			InitState = nullptr;
		}

		Size = state.size();
		TotalSize = Size * eItemSize; 
		InitState = new Item[Size];
		memset(InitState, 0, TotalSize);

		for (uint32_t i = 0; i < Size; i++)
		{
			uint32_t timesOccured = 0u;
			InitState[i].s = state[i];

			for (uint32_t j = 0; j < i; j++)
			if (InitState[i].s == InitState[j].s)
				timesOccured++;

			InitState[i].i = timesOccured;

			/*
			for (uint32_t j = 0; j < i; j++) 
			if (InitState[i].s == InitState[j].s)
			if (InitState[j].imax < timesOccured) 
				InitState[j].imax = timesOccured;
			*/
		}
	}

	void Permut()
	{
		Permutations.clear();

		Permutations.push_back(GenerateString(InitState, Size));
		Permut(1, InitState);
		Permut(0, InitState);
	}

	StringVector Permutations;
	Items InitState;
	uint32_t Size;
	uint32_t TotalSize;

private:

	inline void Swap( _Inout_ Item *items, _In_ int32_t i, _In_ int32_t j ) 
	{ 
		std::swap(items[i], items[j]);
	}

	void Permut(uint32_t s, Item *prev)
	{
		if (s == Size) return; else
		{
			for (uint32_t i = s; i < Size; i++)
			{
				uint32_t timesOccured = 0;
				Items current = new Item[Size];  
				memcpy(current, prev, TotalSize);

				for (uint32_t j = 0; j < s; j++)
				if (prev[i].s == prev[j].s)
					timesOccured++;

				if (timesOccured == prev[i].i && current[s].s != current[i].s)
				{
					Swap(current, s, i); //_Write_ln(current); 
					Permutations.push_back(GenerateString(current, Size));
					Permut(s + 1, current);
				}

				delete [] current;
				current = nullptr;
			}
		}
	}
};

void InitializeProblem( _In_ Problem *problem )
{
	problem->SetInitialState( "XXIO" );

	for (uint32_t i = 0; i < problem->Size; i++)
		problem->InitState[i].Print();
	_Next_ln;
};

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