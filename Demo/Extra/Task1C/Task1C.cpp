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
#define _Sprintf_begin sprintf_s(g_dbg_msg,
#define _Sprintf_end ); _Write_dbg;
#define _Sprintf_end_ln ); _Write_dbg_ln;

class Problem
{
	uint32_t *max_length;
	uint32_t max_seq_len;
	uint32_t max_element_index;

public:

	uint32_t output_seq_sz;
	uint32_t input_seq_sz;

	uint32_t *output_seq;
	uint32_t *input_seq;

	Problem() : 
		output_seq(nullptr), input_seq(nullptr), 
		output_seq_sz(0), input_seq_sz(0)
	{ 
	}

	~Problem()
	{
		delete[] max_length; max_length = nullptr;
		delete[] output_seq; output_seq = nullptr;
		delete[] input_seq; input_seq = nullptr;

	}

	template <typename _SzTy, typename _SeqTy>
	static void PrintSequence(_SzTy sz, _SeqTy *seq)
	{
		stringstream ss; 
		for (decltype(sz) i = 0; i < sz; i++)
		{
			ss << seq[i] << " ";
		}
		{
			string s = ss.str();
			_Write_ln(s.c_str());
		}
	}

	void Solve()
	{
		_Write("Solving...\n");

		max_length = new uint32_t[input_seq_sz];
		ZeroMemory(max_length, sizeof uint32_t * input_seq_sz);

		max_seq_len = 0;
		max_element_index = 0;

		max_length[0] = 1; // no elements on the left side
		for (uint32_t i = 1; i < input_seq_sz; i++) // loop though the all elements [0..sz]
		{
			uint32_t max_len = 1; // always zero

			if (input_seq[i] <  2 || i == 0) { max_length[i] = 1; continue; }
			else for (int32_t j = i - 1; j >= 0; j--) // loop the elements [0..i-1]
			{
				if ((max_len < max_length[j]) && (input_seq[i] > input_seq[j]))
				{
					max_len = max_length[j];
				}

				_Sprintf_begin "in[%d]=%d %c in[%d]=%d / max=%d %c max[%d]=%d\n", 
					i, input_seq[i], input_seq[i] > input_seq[j] ? '>' : '<', j, input_seq[j], 
					max_len, max_len > max_length[j] ? '>' : '<', j, max_length[j]
				_Sprintf_end;
			}

			if (input_seq[max_element_index] < input_seq[i]) 
			{
				max_element_index = i;
			}

			max_length[i] = max_len + 1;
			if (max_seq_len < max_length[i]) 
			{
				max_seq_len = max_length[i];
			}
		}

		_Write("Initial:\t"); PrintSequence(input_seq_sz, input_seq);
		_Write("Charact:\t"); PrintSequence(input_seq_sz, max_length);

		output_seq_sz = max_seq_len;
		output_seq = new uint32_t[output_seq_sz];
		ZeroMemory(output_seq, sizeof uint32_t * output_seq_sz);
		
		int32_t next_output_index = max_seq_len - 1;
		int32_t next_input_index = max_element_index; while (next_output_index >= 0)
		{
			output_seq[next_output_index] = input_seq[next_input_index];

			int32_t next_len = max_length[next_input_index] - 1;
			for (int32_t j = next_input_index - 1; j >= 0; j--)
			{
				if (max_length[j] == next_len) 
				{
					next_input_index = j;
					break;
				}
			}

			next_output_index--;
		}

		_Write("Solution:\t"); PrintSequence(output_seq_sz, output_seq);
	}

};

void InitializeProblem(Problem *problem)
{
	problem->input_seq_sz = 6;

	problem->input_seq = new uint32_t[problem->input_seq_sz];
	problem->input_seq[0] = 2;
	problem->input_seq[1] = 5;
	problem->input_seq[2] = 3;
	problem->input_seq[3] = 4;
	problem->input_seq[4] = 6;
	problem->input_seq[5] = 1;

	// Result: 
	// 4
	// 2 3 4 6
}

void main()
{
	Problem problem;
	InitializeProblem(&problem);
	problem.Solve();
}