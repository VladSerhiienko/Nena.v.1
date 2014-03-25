#pragma once

#include <Windows.h>
#include <math.h>
#include <string>
#include <iostream>
#include <debugapi.h>

#ifndef WIN32_CLEAN_AND_MEAN
#define WIN32_CLEAN_AND_MEAN
#endif

namespace Demo
{
	struct matrix_nxn
	{
		int i = 0, j = 0;
		float **data = nullptr;

		matrix_nxn() {}
		matrix_nxn(const matrix_nxn &other)
		{
			alloc(other.i, other.j);
			for (int x = 0; x < i; x++)
			for (int y = 0; y < j; y++)
				data[i][j] = other.data[i][j];
		}

		~matrix_nxn() { free(); }

		bool free()
		{
			if (data)
			{
				for (int x = 0; x < i; x++)
					delete [] data[x],
					data[x] = nullptr;
				delete [] data,
					data = nullptr;
			}

			return true;
		}

		bool alloc(int x, int y)
		{
			free();
			i = x;
			j = y;

			data = new float *[i];
			if (!data) return false;
			for (x = 0; x < i; x++)
			{
				data[x] = new float[j];
				if (!data[x])
				{
					x--;
					for (; x >= 0; x--)
						delete [] data[x],
						data[x] = nullptr;
					delete [] data,
						data = nullptr;
					return false;
				}
			}

			return true;
		}

		float const *operator[](int x) const
		{
			if (x < i) return data[x];
			else return nullptr;
		}

		float *operator[](int x)
		{
			if (x < i) return data[x];
			else return nullptr;
		}
	};

	struct vector_n : public matrix_nxn
	{
	private:
		using matrix_nxn::operator[];

	public:
		using matrix_nxn::free;
		using matrix_nxn::i;
		using matrix_nxn::j;

		int n;
		bool column;

		vector_n() : matrix_nxn() {}
		vector_n(const vector_n &other) : matrix_nxn(other) {}
		vector_n(const matrix_nxn &other) : matrix_nxn(other) {}

		bool alloc(int size, bool is_column_vector = true)
		{
			n = size;
			column = is_column_vector;
			if (is_column_vector) return matrix_nxn::alloc(size, 1);
			else return matrix_nxn::alloc(1, size);
		}

		float operator[](int x) const
		{
			if (x < i) if (column) return data[x][0];
			return 0.0f;
		}

		float &operator[](int x)
		{
			if (x < i) if (column) return data[x][0]; else  return data[0][x];
			else throw std::exception("out of bound read");
		}
	};

	struct math
	{
		static bool add(matrix_nxn const &a, matrix_nxn const &b, matrix_nxn &c)
		{
			if (a.i != b.i || a.j != b.j) return false;
			if (!c.alloc(a.i, a.j)) return false;
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				c[x][y] = a[x][y] + b[x][y];
		}

		static bool sub(matrix_nxn const &a, matrix_nxn const &b, matrix_nxn &c)
		{
			if (a.i != b.i || a.j != b.j) return false;
			if (!c.alloc(a.i, a.j)) return false;
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				c[x][y] = a[x][y] - b[x][y];
		}

		static bool mul(matrix_nxn const &a, matrix_nxn const &b, matrix_nxn &c)
		{
			if (a.j != b.i) return false;
			if (!c.alloc(a.i, b.j)) return false;
			zero(c);

			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < b.j; y++)
			for (int k = 0; k < a.j; k++)
				c[x][y] += a[x][k] * b[k][y];
			return true;
		}

		static bool mul(matrix_nxn const &a, float const &b, matrix_nxn &c)
		{
			if (!c.alloc(a.i, a.j)) return false;
			for (int x = 0; x < a.i; x++)
			for (int k = 0; k < a.j; k++)
				c[x][k] += a[x][k] * b;
			return true;
		}

		static bool transpose(matrix_nxn const &a, matrix_nxn &b)
		{
			if (!b.alloc(a.j, a.i)) return false;
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				b[x][y] = a[y][x];
			return true;
		}

		static matrix_nxn transpose_new(matrix_nxn const &a)
		{
			matrix_nxn b;
			if (!b.alloc(a.j, a.i)) throw std::exception("memory");
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				b[x][y] = a[y][x];
			return b;
		}

		static bool dot2(vector_n const &a, vector_n const &b, float &c)
		{
			if (a.i != b.j || a.j != b.i) return false;
			matrix_nxn m; if (!mul(a, b, m)) return false;
			if (m.i != 1 && m.j != 1) return false;
			c = m[0][0];
			return true;
		}

		static bool dot(vector_n const &a, vector_n const &b, float &c)
		{
			if (a.n != b.n) return false;
			c = 0.0f;
			for (int x = 0; x < a.n; x++)
				c += a[x] * b[x];
			return true;
		}

		static bool mulew(vector_n const &a, vector_n const &b, vector_n &c)
		{
			if (a.n != b.n) return false;
			if (!c.alloc(a.n)) return false;
			for (int x = 0; x < a.n; x++)
				c[x] = a[x] * b[x];
			return true;
		}

		static bool div_ew(vector_n const &a, vector_n const &b, vector_n &c)
		{
			if (a.n != b.n) return false;
			if (!c.alloc(a.n)) return false;
			for (int x = 0; x < a.n; x++)
				c[x] = a[x] / b[x];
			return true;
		}

		static bool zero(matrix_nxn &a)
		{
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				a[x][y] = 0.0f;
			return true;
		}

		static bool neg(matrix_nxn const &a, matrix_nxn &b)
		{
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				b[x][y] = -a[x][y];
			return true;
		}

		static matrix_nxn neg_new(matrix_nxn const &a)
		{
			matrix_nxn b;
			b.alloc(a.i, a.j);
			for (int x = 0; x < a.i; x++)
			for (int y = 0; y < a.j; y++)
				b[x][y] = -a[x][y];
			return b;
		}

		static bool identity(matrix_nxn &a)
		{
			for (int x = 0; x < a.i; x++)
				a[x][x] = 1;
			return true;
		}

		static void trace(matrix_nxn const &a)
		{
			char s[256] = { 0 };
			for (int x = 0; x < a.i; x++)
			{
				for (int y = 0; y < a.j; y++)
					sprintf_s(s, "%2.2f ", a[x][y]),
					OutputDebugStringA(s);
				OutputDebugStringA("\n");
			}
			OutputDebugStringA("\n");

		}

	};

}