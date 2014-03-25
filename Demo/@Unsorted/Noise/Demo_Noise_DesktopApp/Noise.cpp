#include "Precompiled.h"
#include "Noise.h"

#pragma region Array wrappers

Sandbox::NoiseUtil::NoiseArray2::NoiseArray2(
	_In_ int32 x,
	_In_ int32 y
	) : Array(nullptr), x(x), y(y), cleanup(true)
{
	AllocFloatArray(&Array, x, y);
}

Sandbox::NoiseUtil::NoiseArray2::~NoiseArray2()
{
	if (cleanup) DeallocFloatArray(&Array, x);
}

float32 * Sandbox::NoiseUtil::NoiseArray2::operator [](
	_In_ int32 x
	)
{
	return Array[x];
}

float32 const * Sandbox::NoiseUtil::NoiseArray2::operator [](
	_In_ int32 x
	) const
{
	return Array[x];
}

Sandbox::NoiseUtil::NoiseArray3::NoiseArray3(
	_In_ int32 x,
	_In_ int32 y,
	_In_ int32 z
	) : Array(nullptr), x(x), y(y), z(z), cleanup(true)
{
	AllocFloatArray(&Array, x, y, z);
}

Sandbox::NoiseUtil::NoiseArray3::~NoiseArray3()
{
	if (cleanup) DeallocFloatArray(&Array, x, y);
}

float32 ** Sandbox::NoiseUtil::NoiseArray3::operator [](
	_In_ int32 x
	)
{
	return Array[x];
}

float32 const ** Sandbox::NoiseUtil::NoiseArray3::operator [](
	_In_ int32 x
	) const
{
	return (const float32 **)Array[x];
}

#pragma endregion

#pragma region Utility functions

void Sandbox::NoiseUtil::RandomSeed(int seed)
{
	srand(seed);
}

float32 Sandbox::NoiseUtil::NextRandomFloat()
{
	return _To_single rand() / _To_single RAND_MAX;
}

float32 Sandbox::NoiseUtil::Lerp(float32 x0, float32 x1, float32 t)
{
	return x0 * (1.0f - t) + x1 * t;
}

void Sandbox::NoiseUtil::AllocFloatArray(
	_In_	float32 **arr,
	_In_	int32 len
	)
{
	if (!arr) return;
	(*arr) = new float32[len];
	//ZeroValueArray(alloc, len);
}

void Sandbox::NoiseUtil::AllocFloatArray(
	_In_	float32 ***arr,
	_In_	int32 lenx,
	_In_	int32 leny
	)
{
	if (!arr) return;
	(*arr) = new float32*[lenx];
	ZeroValueArray((*arr), lenx);
	for (decltype(lenx) i = 0; i < lenx; i++)
	{
		(*arr)[i] = new float32[leny];
		ZeroValueArray((*arr)[i], leny);
	}
}

void Sandbox::NoiseUtil::AllocFloatArray(
	_In_	float32 ****arr,
	_In_	int32 lenx,
	_In_	int32 leny,
	_In_	int32 lenz
	)
{
	if (!arr) return;
	(*arr) = new float32**[lenx];
	ZeroValueArray((*arr), lenx);
	for (decltype(lenx) i = 0; i < lenx; i++)
	{
		(*arr)[i] = new float32*[leny];
		ZeroValueArray((*arr)[i], leny);
		for (decltype(leny) j = 0; j < leny; j++)
		{
			(*arr)[i][j] = new float32[lenz];
			ZeroValueArray((*arr)[i][j], lenz);
		}
	}
}

void Sandbox::NoiseUtil::DeallocFloatArray(
	_In_ float32 **arr
	)
{
	auto dealloc = (*arr);  if (!arr) return;
	_Delete_pointer_array(dealloc);
}

void Sandbox::NoiseUtil::DeallocFloatArray(
	_In_ float32 ***arr,
	_In_ int32 lenx
	)
{
	auto dealloc = (*arr); if (!arr) return;
	for (decltype(lenx) i = 0; i < lenx; i++)
		_Delete_pointer_array(dealloc[i]);
	_Delete_pointer_array(dealloc);

}

void Sandbox::NoiseUtil::DeallocFloatArray(
	_In_ float32 ****arr,
	_In_ int32 lenx,
	_In_ int32 leny
	)
{
	auto dealloc = (*arr); if (!arr) return;
	for (decltype(lenx) i = 0; i < lenx; i++)
	{
		for (decltype(leny) j = 0; j < leny; j++)
		{
			_Delete_pointer_array(dealloc[i][j])
		}

		_Delete_pointer_array(dealloc[i])
	}

	_Delete_pointer_array(dealloc)
}

#pragma endregion 

void Sandbox::NoiseUtil::GenerateWhiteNoise(
	_Inout_ float32 ***noise,
	_In_ int32 width,
	_In_ int32 height,
	_In_ RandomProviderFunction random /*= nullptr*/
	)
{
	auto noisePtr = *noise;
	if (!noisePtr) return;
	if (!random) random = Sandbox::NoiseUtil::NextRandomFloat;

	for (auto i = 0; i < width; i++)
		for (auto j = 0; j < height; j++)
			noisePtr[i][j] = random();
}

void Sandbox::NoiseUtil::GenerateSmoothNoise(
	_Inout_	float32 ***smoothNoise,
	_In_ float32 const ***whiteNoise,
	_In_ int32 width,
	_In_ int32 height,
	_In_ int32 octave,
	_In_ InterpolationFunction lerp /*= nullptr*/
	)
{
	if (!lerp) lerp = Sandbox::NoiseUtil::Lerp;
	auto smoothNoisePtr = *smoothNoise;
	auto whiteNoisePtr = *whiteNoise;
	if (!smoothNoisePtr) return;
	if (!whiteNoisePtr) return;

	auto const samplePeriod = 1 << octave; // 2 pow octave
	auto const sampleFrequency = 1.0f / samplePeriod;

	for (decltype(width) i = 0; i < width; i++)
	{
		// Calculate horizontal sampling indices

		int32 const samplei0 = (i / samplePeriod) * samplePeriod;
		int32 const samplei1 = (samplei0 + samplePeriod) % width;
		float32 const horizontalBlend = (i - samplei0) * sampleFrequency;

		for (decltype(height) j = 0; j < height; j++)
		{
			// Calculate vertical sampling indices

			int32 const samplej0 = (j / samplePeriod) * samplePeriod;
			int32 const samplej1 = (samplej0 + samplePeriod) % height;
			float32 const verticalBlend = (j - samplej0) * sampleFrequency;

			// Blend two top corners

			float32 const top = lerp(
				whiteNoisePtr[samplei0][samplej0],
				whiteNoisePtr[samplei1][samplej0],
				horizontalBlend
				);

			// Blend two bottom corners

			float32 const bottom = lerp(
				whiteNoisePtr[samplei0][samplej1],
				whiteNoisePtr[samplei1][samplej1],
				horizontalBlend
				);

			// Final blend

			smoothNoisePtr[i][j] = lerp(
				top,
				bottom,
				verticalBlend
				);
		}
	}
}

void Sandbox::NoiseUtil::GeneratePerlinNoise(
	_Inout_	float32 ***perlinNoise,
	_In_ float32 const ***whiteNoise,
	_In_ int32 width,
	_In_ int32 height,
	_In_ int32 octaveCount,
	_In_ InterpolationFunction lerp /*= nullptr*/
	)
{
	auto perlinNoisePtr = *perlinNoise;
	if (!perlinNoisePtr) return;

	NoiseArray3 smoothNoise(octaveCount, width, height);
	for (decltype(octaveCount) octave = 0; octave < octaveCount; octave++)
	{
		GenerateSmoothNoise(
			&smoothNoise.Array[octave], whiteNoise,
			width, height, 
			octave,
			lerp
			);
	}

	float32 amplitude = 1.0f;
	float32 persistance = 0.7f;
	float32 totalAmplitude = 0.0f;

	for (decltype(octaveCount) octave = octaveCount - 1; octave >= 0; octave--)
	{
		amplitude *= persistance;
		totalAmplitude += amplitude;

		for (decltype(width) i = 0; i < width; i++)
			for (decltype(height) j = 0; j < height; j++)
				perlinNoisePtr[i][j] += smoothNoise[octave][i][j] * amplitude;
	}

	for (decltype(width) i = 0; i < width; i++)
		for (decltype(height) j = 0; j < height; j++)
			perlinNoisePtr[i][j] /= totalAmplitude;
}