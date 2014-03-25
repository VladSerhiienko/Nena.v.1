#pragma once

#include "mtx.h"
#include <vector>

namespace Demo
{
	struct dfp
	{
		typedef float(*func)(vector_n const &x);

		func obj;
		std::vector<func> objdiff;

		vector_n gk(vector_n const &x)
		{
			int sz = (int) objdiff.size();

			vector_n diffs;
			diffs.alloc(sz, false);
			for (int i = 0; i < sz; i++)
				diffs[i] = objdiff[i](x);
			return diffs;
		}

		vector_n pk(matrix_nxn const &V, vector_n const &g)
		{
			matrix_nxn Vneg = math::neg_new(V);
			vector_n p; math::mul(Vneg, g, p);
			return p;
		}

		vector_n xk(vector_n const &x, vector_n const &p, vector_n const &a)
		{
			vector_n ap, xn;
			math::mulew(a, p, ap);
			math::add(x, ap, xn);
			return xn;

		}

		vector_n yk(vector_n const &x1, vector_n const &x0)
		{
			vector_n g1 = gk(x1), g0 = gk(x0), y;
			math::sub(g1, g0, y);
			return y;
		}

		matrix_nxn Ak(matrix_nxn const &V, vector_n const &y)
		{
			/*
			up = (V*y*y'*V);
			down = (y'*V*y);
			result = up/down;
			*/

			vector_n yt = math::transpose_new(y);
			float yyt; math::dot(y, yt, yyt);
			matrix_nxn Vyyt; math::mul(V, yyt, Vyyt);
			matrix_nxn VyytV; math::mul(Vyyt, V, VyytV);



			return VyytV;
		}

		void operator()(vector_n &result)
		{

		}
	};
}