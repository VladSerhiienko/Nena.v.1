#pragma once
#include "Precompiled.h"

namespace Sandbox
{
	// Perlin noise helper functions and types
	namespace PerlinNoiseUtil
	{
		// = 3t2-2t3 for t[0;1]
		REIGN_HINTINLINING static float32 Scurve(
			_In_ float32 t
			)
		{
			return t * t * (3.0f - 2.0f * t);
		}

		// = a + (b-a)t 
		REIGN_HINTINLINING static float32 Lerp(
			_In_ float32 t,
			_In_ float32 a,
			_In_ float32 b
			)
		{
			return a + (b - a) * t;
		}

		// Default static noise parameters
		struct PerlinNoiseDefaults
		{
			static const uint32 DefaultNoiseBase = 0x100;
			static const uint32 DefaultNoiseSeed = 0x001;
			static const uint32 DefaultNoiseMask = 0x0ff;
		};

		template <typename TGradient>
		// Perlin noise basic types that implements permutation array init, 
		// dynamic memory alloc/dealloc, common properties and functions. 
		class PerlinNoiseBase : public PerlinNoiseDefaults
		{
		public:

			typedef TGradient NoiseGradient;
			enum { NumDimensions = sizeof NoiseGradient / 4 };

			virtual ~PerlinNoiseBase()
			{
				_Delete_pointer_array(m_permutation);
				_Delete_pointer_array(m_grad);
			}

			virtual float32 SmoothNoise(NoiseGradient sample)
			{
				return 0.0f;
			}

			virtual float32 PerlinNoise(NoiseGradient sample, uint32 octaves)
			{
				auto output = 0.0f, amplitude = 1.0f, scaling = octaves * 16.0f;
				for (auto i = 0u; i < octaves; i++, scaling /= 2.0f, amplitude = 1.0f / i)
					output += amplitude * SmoothNoise(sample / scaling);

				return output;
			}

			virtual void Init()
			{
				auto i(0u), j(0u), k(0u);

				for (i = 0; i < m_base; i++)
					m_permutation[i] = i;

				while (--i)
				{
					k = m_permutation[i];
					j = rand() % m_base;
					m_permutation[i] = m_permutation[j];
					m_permutation[j] = k;
				}

				for (i = 0; i < m_base + 2; i++)
					m_permutation[m_base + i] = 
						m_permutation[i];
			}

		public:

			// Pseudorand
			REIGN_HINTINLINING void Seed(_In_ uint32 value) { srand(value); }

			// Get the mask to clamp array indices to noise base
			REIGN_HINTINLINING uint32 Mask() { return m_mask; }
			// Set the mask to clamp array indices to noise base (Base() - 1)
			REIGN_HINTINLINING void Mask(_In_ uint32 value) { m_mask = value; }

			// Get the noise base
			REIGN_HINTINLINING uint32 Base() { return m_base; }
			// Set the noise base, re-allocate permutation and grad arrays
			// and initialize noise parameters if needed.
			REIGN_HINTINLINING void Base(_In_ uint32 value)
			{
				m_base = value;
				_Delete_pointer_array(m_permutation);
				_Delete_pointer_array(m_grad);
				m_permutation = alloc_new int32[m_base * 2 + 2];
				m_grad = alloc_new NoiseGradient[m_base * 2 + 2];
			}

		protected:

			PerlinNoiseBase() :
				m_permutation(nullptr),
				m_grad(nullptr),
				m_base(0u),
				m_mask(0u)
			{
				Seed(DefaultNoiseSeed);
				Mask(DefaultNoiseMask);
				Base(DefaultNoiseBase);
			}

		protected:

			NoiseGradient *m_grad; // Gradients
			int32 *m_permutation; // Permut

			uint32 m_base; // Noise base
			uint32 m_mask; // Clamp mask

		};

		typedef PerlinNoiseUtil::PerlinNoiseBase<Reign::Vector1> PerlinNoise1Base;
		typedef PerlinNoiseUtil::PerlinNoiseBase<Reign::Vector2> PerlinNoise2Base;
		typedef PerlinNoiseUtil::PerlinNoiseBase<Reign::Vector3> PerlinNoise3Base;

	};


	class PerlinNoise1 : public PerlinNoiseUtil::PerlinNoise1Base
	{
	public:

		virtual void Init(_No_args_) override;
		virtual float32 SmoothNoise(_In_ Reign::Vector1 sample) override;

	public:

		PerlinNoise1();
		virtual ~PerlinNoise1();

	};

	class PerlinNoise2 : public PerlinNoiseUtil::PerlinNoise2Base
	{
	public:

		virtual void Init(_No_args_) override;
		virtual float32 SmoothNoise(_In_ Reign::Vector2 sample) override;

	public:

		PerlinNoise2();
		virtual ~PerlinNoise2();

	};

	class PerlinNoise3 : public PerlinNoiseUtil::PerlinNoise3Base
	{
	public:

		virtual void Init(_No_args_) override;
		virtual float32 SmoothNoise(_In_ Reign::Vector3 sample) override;

	public:

		PerlinNoise3();
		virtual ~PerlinNoise3();

	};

};