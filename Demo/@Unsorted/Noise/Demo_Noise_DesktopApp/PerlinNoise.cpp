#include "Precompiled.h"
#include "PerlinNoise.h"

static int random_in_range(int min, int max) {
	return ((double) rand() / ((double) RAND_MAX + 1.0)) * 
		(max - min + 1) + 
		min;
}

#pragma region Perlin noise 1

Sandbox::PerlinNoise1::PerlinNoise1() : PerlinNoiseBase()
{
}

Sandbox::PerlinNoise1::~PerlinNoise1()
{
}

void Sandbox::PerlinNoise1::Init()
{
	PerlinNoiseBase::Init();

	for (auto i = 0u; i < m_base; i++)
	{
		m_grad[i] = _As_single((rand() % (m_base * 2)) - m_base) / m_base;
	}

	for (auto i = 0u; i < m_base + 2; i++)
	{
		m_grad[m_base + i] = m_grad[i];
	}
}

float32 Sandbox::PerlinNoise1::SmoothNoise(Reign::Vector1 x)
{
	using namespace PerlinNoiseUtil;

	// Enclosing basis points
	_Auto_const x0 = (_To_int x) % m_base; // & m_mask; ? 
	_Auto_const x1 = (x0 + 1) % m_base; // & m_mask; ? 

	// Distance to each basis point
	_Auto_const rx0 = x - _To_single _To_int x;
	_Auto_const rx1 = rx0 - 1.0f;

	// Smoothed input
	_Auto_const sx = Scurve(rx0);

	// Perform dot product with the grads to find two input values for the interpolation
	_Auto_const u = rx0 * m_grad[m_permutation[x0]];
	_Auto_const v = rx1 * m_grad[m_permutation[x1]];

	// Return interpolated value
	return Lerp(sx, u, v);
}

#pragma endregion Implementation

#pragma region Perlin noise 2

Sandbox::PerlinNoise2::PerlinNoise2() : PerlinNoiseBase()
{
}

Sandbox::PerlinNoise2::~PerlinNoise2()
{
}

void Sandbox::PerlinNoise2::Init()
{
	PerlinNoiseBase::Init();

	for (auto i = 0u; i < m_base; i++)
	{
		m_grad[i].x = _As_single((rand() % (m_base + m_base)) - m_base) / m_base;
		m_grad[i].y = _As_single((rand() % (m_base + m_base)) - m_base) / m_base;
		m_grad[i].Normalize();


	}

	for (auto i = 0u; i < m_base + 2; i++)
	{
		m_grad[m_base + i].x = m_grad[i].x;
		m_grad[m_base + i].y = m_grad[i].y;
	}
}

float32 Sandbox::PerlinNoise2::SmoothNoise(Reign::Vector2 sample)
{
	using namespace PerlinNoiseUtil;

	// Enclosing x basis points
	_Auto_const x0 = (_To_int sample.x) & m_base;// &m_mask; // & m_mask; ? 
	_Auto_const x1 = (x0 + 1) & m_base;// & m_mask; // & m_mask; ? 

	// Distance to each x basis point
	_Auto_const rx0 = sample.x - _To_int sample.x;
	_Auto_const rx1 = rx0 - 1.0f;

	// Enclosing y basis points
	_Auto_const y0 = (_To_int sample.y) & m_base;// & m_mask; // & m_mask; ? 
	_Auto_const y1 = (y0 + 1) & m_base;// & m_mask;; // & m_mask; ? 

	// Distance to each y basis point
	_Auto_const ry0 = sample.y - _To_int sample.y;
	_Auto_const ry1 = ry0 - 1.0f;

	// Permut the gradients indices using one permutation for each input axis
	_Auto_const b00 = m_permutation[m_permutation[x0] + y0];
	_Auto_const b10 = m_permutation[m_permutation[x1] + y0];
	_Auto_const b01 = m_permutation[m_permutation[x0] + y1];
	_Auto_const b11 = m_permutation[m_permutation[x1] + y1];

	// Get the smoothed interpolation input

	auto u(0.0f), v(0.0f);
	_Auto_const sx = Scurve(rx0);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b00].x + ry0 * m_grad[b00].y;
	v = rx1 * m_grad[b10].x + ry0 * m_grad[b10].y;
	_Auto_const a = Lerp(sx, u, v);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b01].x + ry1 * m_grad[b01].y;
	v = rx1 * m_grad[b11].x + ry1 * m_grad[b11].y;
	_Auto_const b = Lerp(sx, u, v);

	_Auto_const sy = Scurve(ry0);
	return Lerp(sy, a, b);
}

#pragma endregion Implementation

#pragma region Perlin noise 3

Sandbox::PerlinNoise3::PerlinNoise3() : PerlinNoiseBase()
{
}

Sandbox::PerlinNoise3::~PerlinNoise3()
{
}

void Sandbox::PerlinNoise3::Init()
{
	PerlinNoiseBase::Init();

	for (auto i = 0u; i < m_base; i++)
	{
		m_grad[i].x = _To_single ((rand() % (m_base + m_base)) - m_base) / m_base;
		m_grad[i].y = _To_single ((rand() % (m_base + m_base)) - m_base) / m_base;
		m_grad[i].z = _To_single((rand() % (m_base + m_base)) - m_base) / m_base;
		m_grad[i].Normalize();
	}

	for (auto i = 0u; i < m_base + 2; i++)
	{
		m_grad[m_base + i].x = m_grad[i].x;
		m_grad[m_base + i].y = m_grad[i].y;
		m_grad[m_base + i].z = m_grad[i].z;
	}
}

float32 Sandbox::PerlinNoise3::SmoothNoise(Reign::Vector3 sample)
{
	using namespace PerlinNoiseUtil;

	// Enclosing x basis points
	_Auto_const x0 = (_To_int sample.x) & m_mask; // & m_mask; ? 
	_Auto_const x1 = (x0 + 1) & m_mask; // & m_mask; ? 

	// Distance to each x basis point
	_Auto_const rx0 = sample.x - _To_single _To_int sample.x;
	_Auto_const rx1 = rx0 - 1.0f;

	// Enclosing y basis points
	_Auto_const y0 = (_To_int sample.x) & m_mask; // & m_mask; ? 
	_Auto_const y1 = (y0 + 1) & m_mask;; // & m_mask; ? 

	// Distance to each y basis point
	_Auto_const ry0 = sample.y - _To_single _To_int sample.y;
	_Auto_const ry1 = ry0 - 1.0f;

	// Enclosing z basis points
	_Auto_const z0 = (_To_int sample.z) & m_mask; // & m_mask; ? 
	_Auto_const z1 = (z0 + 1) & m_mask; // & m_mask; ? 

	// Distance to each z basis point
	_Auto_const rz0 = sample.z - _To_single _To_int sample.z;
	_Auto_const rz1 = rz0 - 1.0f;

	// Permut the gradients indices using one permutation for each input axis
	_Auto_const b000 = m_permutation[m_permutation[m_permutation[x0] + y0] + z0];
	_Auto_const b010 = m_permutation[m_permutation[m_permutation[x0] + y1] + z0];
	_Auto_const b001 = m_permutation[m_permutation[m_permutation[x0] + y0] + z1];
	_Auto_const b011 = m_permutation[m_permutation[m_permutation[x0] + y1] + z1];
	_Auto_const b100 = m_permutation[m_permutation[m_permutation[x1] + y0] + z0];
	_Auto_const b110 = m_permutation[m_permutation[m_permutation[x1] + y1] + z0];
	_Auto_const b101 = m_permutation[m_permutation[m_permutation[x1] + y0] + z1];
	_Auto_const b111 = m_permutation[m_permutation[m_permutation[x1] + y1] + z1];

	// Get a smoothed interpolation input
	_Auto_const sx = Scurve(rx0);

	auto u(0.0f), v(0.0f);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b000].x + ry0 * m_grad[b000].y + rz0 * m_grad[b000].z; // b000
	v = rx1 * m_grad[b100].x + ry0 * m_grad[b100].y + rz0 * m_grad[b100].z; // b100
	_Auto_const a = Lerp(sx, u, v);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b010].x + ry1 * m_grad[b010].y + rz0 * m_grad[b010].z; // b010
	v = rx1 * m_grad[b110].x + ry1 * m_grad[b110].y + rz0 * m_grad[b110].z; // b110
	_Auto_const b = Lerp(sx, u, v);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b001].x + ry0 * m_grad[b001].y + rz1 * m_grad[b001].z; // b001
	v = rx1 * m_grad[b101].x + ry0 * m_grad[b101].y + rz1 * m_grad[b101].z; // b101
	_Auto_const c = Lerp(sx, u, v);

	// Perform dot product with the grads to find two input values for the interpolation
	u = rx0 * m_grad[b011].x + ry1 * m_grad[b011].y + rz1 * m_grad[b011].z; // b011
	v = rx1 * m_grad[b111].x + ry1 * m_grad[b111].y + rz1 * m_grad[b111].z; // b111
	_Auto_const d = Lerp(sx, u, v);

	_Auto_const sy = Scurve(ry0);
	_Auto_const sz = Scurve(rz0);

	return Lerp(
		sz, // t
		Lerp(sy, a, b), // a
		Lerp(sy, c, d) // b
		);
}

#pragma endregion Implementation