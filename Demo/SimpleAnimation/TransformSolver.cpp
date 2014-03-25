#include "TransformSolver.h"

#define __sqrt3 1.7320508075688772935274463415059f
#define __1over3 0.33333333333333333333333333333333f
#define __1over4 0.25000000000000000000000000000000f
#define __1over2 0.50000000000000000000000000000000f
#define __sqrt3times1over2 __sqrt3 * __1over2

Demo::TransformSolver::Result __TransformSolver_CallConv Demo::TransformSolver::SolveSegment(
	_In_::Nena::Vector2 p1, _In_::Nena::Vector2 m1, // left
	_In_::Nena::Vector2 p2, _In_::Nena::Vector2 m2, // right
	_In_::FLOAT x, _Out_::FLOAT &y
	)
{

	Result result;
	::FLOAT a, b, c, d, xx, yy, zz, t;

	a = 2.0f * p1.x + m1.x - 2.0f * p2.x + m2.x;
	b = -3.0f * p1.x - 2.0f * m1.x + 3.0f * p2.x - m2.x;
	c = m1.x;
	d = p1.x - x;

	result = Solve3(a, b, c, d, xx, yy, zz);
	if (result > 0 && FindHermiteTime(xx, yy, zz, t))
	{
		y = SolveHermiteSegment(
			p1.y, m1.y, // left
			p2.y, m2.y, // right
			t
			);

		return Result::k1Real;
	}

	return Result::kNoReal;
}

Demo::TransformSolver::Result __TransformSolver_CallConv Demo::TransformSolver::SolveSegmentEst(
	_In_::Nena::Vector2 p1, _In_::Nena::Vector2 m1, // left
	_In_::Nena::Vector2 p2, _In_::Nena::Vector2 m2, // right
	_In_::FLOAT x, _Out_::FLOAT &y
	)
{
	::FLOAT const q = p2.x - p1.x;
	::FLOAT const r = x - p1.x;
	::FLOAT const t = r / q;

	y = SolveHermiteSegment(
		p1.y, m1.y, // left
		p2.y, m2.y, // right
		t
		);

	return Result::k1Real;
}

::FLOAT __TransformSolver_CallConv Demo::TransformSolver::SolveHermiteSegment(
	_In_::FLOAT p0, _In_::FLOAT m0, // left
	_In_::FLOAT p1, _In_::FLOAT m1, // right
	_In_::FLOAT t
	)
{
	::FLOAT const t2 = t * t;
	::FLOAT const t3 = t2 * t;
	::FLOAT const h1 = 2 * t3 - 3 * t2 + 1;
	::FLOAT const h2 = t3 - 2 * t2 + t;
	::FLOAT const h3 = -2 * t3 + 3 * t2;
	::FLOAT const h4 = t3 - t2;

	return h1 * p0 + h2 * m0 + h3 * p1 + h4 * m1;
}

Demo::TransformSolver::Result __TransformSolver_CallConv Demo::TransformSolver::Solve2(
	_In_::FLOAT a, _In_::FLOAT b, _In_::FLOAT c,
	_Out_::FLOAT &x, _Out_::FLOAT &y
	)
{
	::FLOAT d = b * b + 4 * a * c; 

	if (d < 0.0f)
	{
		return Result::kNoReal; 
	}
	else if (d == 0.0f)
	{
		d = sqrtf(d);
		x = y = -b * __1over2 / a;
		return Result::k2RealEq;
	}
	else
	{
		d = sqrtf(d);
		x = (-b + d) * 0.5f / a;
		y = (-b - d) * 0.5f / a;
		return Result::k2Real;
	}
}

Demo::TransformSolver::Result __TransformSolver_CallConv Demo::TransformSolver::Solve3(
	_In_::FLOAT a, _In_::FLOAT b, _In_::FLOAT c, _In_::FLOAT d,
	_Out_::FLOAT &x, _Out_::FLOAT &y, _Out_::FLOAT &z
	)
{
	Result result;

	if (a == 0.0f)
	{
		result = Solve2(b, c, d, x, y);
		return result;
	}

	::FLOAT const inv3 = __1over3;
	::FLOAT const inva = 1.0f / a;
	::FLOAT const inva2 = inva / a;
	::FLOAT const inva3 = inva2 / a;
	::FLOAT const b2 = b * b;
	::FLOAT const b3 = b2 * b;

	::FLOAT const f = c / a - b2 * inva2 * inv3;
	::FLOAT g = 2.0f * b3 * inva3 - 9.0f * b * c * inva2 + 27.0f * d * inva; 
	g /= 27.0f;

	::FLOAT const h = g * g * __1over4 + f * f * f / 27.0f;

	if (f == 0.0f && g == 0.0f && h == 0.0f) result = Result::k3RealEq; else
	if (h <= 0) result = Result::k3Real; else
	if (h > 0) result = Result::k1Real;

	::FLOAT i, j, k, L, M, N, P, R, S, T, U;

	i = g * g * 0.2500f - h; 
	i = Power(i, 0.5000f);

	switch (result)
	{

	case Result::k3Real:
		j = Power(i, inv3);
		k = ::acosf(-g * __1over4 / i);
		L = -j;
		M = ::cos(k * inv3);
		N = __sqrt3 * ::sin(k * inv3);
		P = b * -inv3 / a;
		x = 2 * j * ::cos(k * inv3) - (b * inv3 / a);
		y = L * (M + N) + P;
		z = L * (M - N) + P;
		break;

	case Result::k3RealEq:
		x = y = z = -Power(d / a, inv3);
		break;

	case Result::k1Real:
		R = g * -__1over2 + ::sqrtf(h); S = Power(R, inv3);
		T = g * -__1over2 - ::sqrtf(h); U = Power(T, inv3);
		x = S + U - b * inv3 / a; if (!::isnan(x)) { y = z = x; break; }
		y = -__1over2 * (S + U) - b * inv3 / a + i * (S - U) * __sqrt3times1over2; if (!::isnan(y)) { x = z = y; break; }
		z = -__1over2 * (S + U) - b * inv3 / a - i * (S - U) * __sqrt3times1over2; if (!::isnan(z)) { y = x = z; break; }
		break;

	}

	return result;
}

::FLOAT __TransformSolver_CallConv Demo::TransformSolver::Power(
	_In_::FLOAT x, _In_::FLOAT p
	)
{
	return x < 0.0f ? -::powf(-x, p) : ::powf(x, p);
}

::FLOAT __TransformSolver_CallConv Demo::TransformSolver::Max(
	_In_::FLOAT x, _In_::FLOAT y
	)
{
	return max(x, y);
}

::FLOAT __TransformSolver_CallConv Demo::TransformSolver::Max(
	_In_::FLOAT x, _In_::FLOAT y, _In_::FLOAT z
	)
{
	return max(max(x, y), max(y, z));
}

::BOOL __TransformSolver_CallConv Demo::TransformSolver::FindHermiteTime(
	_In_::FLOAT x, _In_::FLOAT y, _In_::FLOAT z,
	_Out_::FLOAT &t
	)
{
	if (x <= 1.0f && x >= 0.0f) { t = x; return TRUE; }
	if (y <= 1.0f && y >= 0.0f) { t = y; return TRUE; }
	if (z <= 1.0f && z >= 0.0f) { t = z; return TRUE; }

	return FALSE;
}