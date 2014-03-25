#pragma once
#include "Precompiled.h"

namespace Sandbox
{
	class NoiseUtil
	{
	public:

		struct NoiseArray2
		{
			bool cleanup; // deallocate memory in dtor
			int32 x, y; // array dimensions
			float32 ** Array; // actual array

			float32 const * operator [](
				_In_ int32 x
				) const;
			float32 *operator [](
				_In_ int32 x
				);

			NoiseArray2(
				_In_ int32 x, // width
				_In_ int32 y // height
				);
			~NoiseArray2(
				);
		};

		struct NoiseArray3
		{
			bool cleanup; // deallocate memory in dtor
			int32 x, y, z; // array dimensions
			float32 *** Array; // actual array

			float32 const ** operator [](
				_In_ int32 x
				) const;
			float32 ** operator [](
				_In_ int32 x
				);

			NoiseArray3(
				_In_ int32 x, // num octaves
				_In_ int32 y, // width
				_In_ int32 z // height
				);
			~NoiseArray3(
				);
		};

		typedef float32 (*InterpolationFunction)(float32, float32, float32);
		typedef float32 (*RandomProviderFunction)();

	public:

		static void RandomSeed(int32 seed);
		static float32 NextRandomFloat();

		static void AllocFloatArray(
			_In_	float32 **arr,
			_In_	int32 len
			);
		static void AllocFloatArray(
			_In_	float32 ***arr,
			_In_	int32 lenx,
			_In_	int32 leny
			);
		static void AllocFloatArray(
			_In_	float32 ****arr,
			_In_	int32 lenx,
			_In_	int32 leny,
			_In_	int32 lenz
			);

		static void DeallocFloatArray(
			_In_	float32 **arr
			);
		static void DeallocFloatArray(
			_In_	float32 ***arr,
			_In_	int32 lenx
			);
		static void DeallocFloatArray(
			_In_	float32 ****arr,
			_In_	int32 lenx,
			_In_	int32 leny
			);

		static float32 Lerp(
			_In_	float32 x0,
			_In_	float32 x1,
			_In_	float32 t
			);

		static void GenerateWhiteNoise(
			_Inout_ float32 ***noise,
			_In_	int32 width,
			_In_	int32 height,
			_In_	RandomProviderFunction random = nullptr
			);
		static void GenerateSmoothNoise(
			_Inout_	float32 ***smoothNoise, // final noise 
			_In_	float32 const ***whiteNoise, // noise base
			_In_	int32 width,
			_In_	int32 height,
			_In_	int32 octave,
			_In_	InterpolationFunction lerp = nullptr
			);
		static void GeneratePerlinNoise(
			_Inout_	float32 ***perlinNoise,
			_In_	float32 const ***whiteNoise, // noise base
			_In_	int32 width,
			_In_	int32 height,
			_In_	int32 octaveCount, // num octaves to blend
			_In_	InterpolationFunction lerp = nullptr
			);

		static REIGN_HINTINLINING void GenerateWhiteNoise(
			_Inout_ NoiseArray2 * noise,
			_In_	RandomProviderFunction random = nullptr
			)
		{
			GenerateWhiteNoise(
				&noise->Array, 
				noise->x, 
				noise->y,
				random
				);
		}

		static REIGN_HINTINLINING void GenerateSmoothNoise(
			_Inout_	NoiseArray2 * smoothNoise, // final noise 
			_In_	NoiseArray2 const * whiteNoise, // noise base
			_In_	int32 octave,
			_In_	InterpolationFunction lerp = nullptr
			)
		{
			GenerateSmoothNoise(
				&smoothNoise->Array,
				(const float32 ***) &whiteNoise->Array,
				smoothNoise->x, 
				smoothNoise->y,
				octave,
				lerp
				);
		}

		static REIGN_HINTINLINING void GeneratePerlinNoise(
			_Inout_	NoiseArray2 * perlinNoise, // final noise 
			_In_	NoiseArray2 const * whiteNoise, // noise base
			_In_	int32 octaveCount, // num octaves to blend
			_In_	InterpolationFunction lerp = nullptr
			)
		{
			GeneratePerlinNoise(
				&perlinNoise->Array,
				(const float32 ***) &whiteNoise->Array,
				perlinNoise->x,
				perlinNoise->y,
				octaveCount,
				lerp
				);
		}


	};
};