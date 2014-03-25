#include <iostream>
#include <math.h>

static float Hermite(float t, float p0, float p1, float m0, float m1)
{
	float t2 = t * t; float t3 = t2 * t;
	float h1 = 2 * t3 - 3 * t2 + 1;
	float h2 = t3 - 2 * t2 + t;
	float h3 = -2 * t3 + 3 * t2;
	float h4 = t3 - t2;

	return h1 * p0 + h2 * m0 + h3 * p1 + h4 * m1;
}

// IEEE standard hack :3
static inline float Power(float x, float p)
{
	return x < 0.0f ? -powf(-x, p) : powf(x, p);
}

static inline float Max(float x, float y)
{
	return x > y ? x : y;
}

static inline float Max(float x, float y, float z)
{
	return Max(Max(x, y), Max(y, z));
}

static inline bool FindHermiteTime(float x, float y, float z, float &t)
{
	if (x <= 1.0f && x >= 0.0f) { t = x; return true; }
	if (y <= 1.0f && y >= 0.0f) { t = y; return true; }
	if (z <= 1.0f && z >= 0.0f) { t = z; return true; }
	return false;
}

enum SOLVER_RESULT
{
	SOLVER_RESULT_IS_UNDEFINED,
	SOLVER_RESULT_IS_3_REAL_ROOTS,
	SOLVER_RESULT_IS_2_REAL_ROOTS,
	SOLVER_RESULT_IS_3_REAL_EQUAL_ROOTS,
	SOLVER_RESULT_IS_2_REAL_EQUAL_ROOTS,
	SOLVER_RESULT_IS_1_REAL_ROOT,
	SOLVER_RESULT_IS_NO_REAL_ROOTS,
};

struct SquareSolver
{
	float a, b, c;
	float x, y;

	SOLVER_RESULT operator()()
	{
		SOLVER_RESULT result;

		float d = b * b + 4 * a * c;

		if (d < 0.0f) result = SOLVER_RESULT_IS_NO_REAL_ROOTS; else 
		if (d == 0.0f)
		{
			d = sqrtf(d);
			x = y = -b * 0.5f / a;
			result = SOLVER_RESULT_IS_2_REAL_EQUAL_ROOTS;
		}
		else
		{
			d = sqrtf(d);
			x = (-b + d) * 0.5f / a;
			y = (-b - d) * 0.5f / a;
			result = SOLVER_RESULT_IS_2_REAL_EQUAL_ROOTS;
		}

		return result;
	}

};

// ax3 + bx2 + cx + d = 0;
struct CubicSolver
{

	float a, b, c, d;
	float x, y, z;

	SOLVER_RESULT operator()()
	{
		SOLVER_RESULT result;

		if (a == 0.0f)
		{
			SquareSolver ss; 
			ss.a = b;
			ss.b = c;
			ss.c = d;

			result = ss();
			x = ss.x;
			y = ss.y;

			return result;
		}

		float inv3 = 1.0f / 3.0f;
		float inva = 1.0f / a;
		float inva2 = inva / a;
		float inva3 = inva2 / a;
		float b2 = b * b;
		float b3 = b2 * b;

		float f = c / a - b2 * inva2 * inv3;
		float g = 2 * b3 * inva3 - 9 * b * c * inva2 + 27 * d * inva; g /= 27.0f;
		float h = g * g * 0.25f + f * f * f / 27.0f;

		if (f == 0.0f && g == 0.0f && h == 0.0f) result = SOLVER_RESULT_IS_3_REAL_EQUAL_ROOTS; else
		if (h <= 0) result = SOLVER_RESULT_IS_3_REAL_ROOTS; else
		if (h > 0) result = SOLVER_RESULT_IS_1_REAL_ROOT;

		float i, j, k, L, M, N, P, R, S, T, U;
		i = g * g * 0.2500f - h; i = Power(i, 0.5000f);

		switch (result)
		{
		case SOLVER_RESULT_IS_3_REAL_ROOTS:
			j = Power(i, inv3);
			k = acosf(-g * 0.5000f / i);
			L = -j;
			M = cos(k * inv3);
			N = 1.7320f * sin(k * inv3);
			P = b * -inv3 / a;
			x = 2 * j * cos(k * inv3) - (b * inv3 / a);
			y = L * (M + N) + P;
			z = L * (M - N) + P;
			break;
		case SOLVER_RESULT_IS_3_REAL_EQUAL_ROOTS:
			x = y = z = -Power(d / a, inv3);
			break;
		case SOLVER_RESULT_IS_1_REAL_ROOT:
			R = g * -0.5f + sqrtf(h); S = Power(R, inv3);
			T = g * -0.5f - sqrtf(h); U = Power(T, inv3);
			x = S + U - b * inv3 / a; if (!isnan(x)) { y = z = x; break; }
			y = -0.5000f * (S + U) - b * inv3 / a + i * (S - U) * sqrtf(3.0000f) * 0.5000f; if (!isnan(y)) { x = z = y; break; }
			z = -0.5000f * (S + U) - b * inv3 / a - i * (S - U) * sqrtf(3.0000f) * 0.5000f; if (!isnan(z)) { y = x = z; break; }
			break;
		}

		return result;
	}
};

static void TestCubicSolver()
{
	CubicSolver cs;
	cs.a = 2;
	cs.b = -4;
	cs.c = -22;
	cs.d = 24;

	auto result = cs();

	cs.a = 3;
	cs.b = -10;
	cs.c = 14;
	cs.d = 27;

	result = cs();

	cs.a = 1;
	cs.b = 6;
	cs.c = 12;
	cs.d = 8;

	result = cs();
}

struct float2
{
	float x, y;
};

struct HermiteCurveSolver
{
	float2 p1, p2, m1, m2;

	float Exact(float x)
	{
		float a = 2 * p1.x + m1.x - 2 * p2.x + m2.x;
		float b = -3 * p1.x - 2 * m1.x + 3 * p2.x - m2.x;
		float c = m1.x;
		float d = p1.x - x;

		CubicSolver cs;
		cs.a = a;
		cs.b = b;
		cs.c = c;
		cs.d = d;
		cs();

		float time; bool result = FindHermiteTime(cs.x, cs.y, cs.z, time);
		float value = Hermite(time, p1.y, p2.y, m1.y, m2.y);
		return value;
	}

	float Approx(float x)
	{
		float q = p2.x - p1.x; float r = x - p1.x;
		float t = r / q;
		float value = Hermite(t, p1.y, p2.y, m1.y, m2.y);
		return value;
	}


};

static void TestHermiteSolver()
{
	HermiteCurveSolver hs;
	memset(&hs, 0, sizeof hs);
	hs.m1.x = hs.m2.x = 0;
	hs.p1.x = hs.p1.y = hs.p2.y = 0;
	hs.m2.y = 1;
	hs.m1.y = 1;
	hs.p2.x = 5;

	char msg[512]; sprintf_s(msg, "x=exact => y=exact (apprx)\n");
	std::cout << msg << "--------------------------\n";

	for (float x = 0.000f; x <= 5.000f; x += 0.050f)
	{
		float exact = hs.Exact(x), approx = hs.Approx(x);
		sprintf_s(msg, "x=%2.3f => y=%2.3f (%2.3f)\n", x, exact, approx);
		std::cout << msg;
	}

}

int main()
{
	TestHermiteSolver();

	getchar();
	return 0;
}