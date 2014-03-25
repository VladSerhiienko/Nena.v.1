#pragma once

#include <vector>
#include <unordered_map>

#include <Nena\Math.h>
#include <Nena\PlatformAssist.h>

namespace Demo
{
	struct Transform
	{
		typedef struct Curve
		{
			typedef enum Index
			{
				kSx, kSy, kSz,
				kRx, kRy, kRz,
				kTx, kTy, kTz,
				kMax
			} CurveIndex;

			typedef Curve *SXArray[kMax];

			typedef ::std::string String;
			typedef ::std::unordered_map<String, Curve *> MapToString;

			typedef ::std::vector<Curve*> Vector;
			typedef Vector::iterator Iterator;

			typedef struct Key
			{
				typedef ::std::vector<Key*> Vector;
				typedef Vector::iterator Iterator;

				::Nena::Vector2 TV; //!< \var Time (ms) & value (m)
				::Nena::Vector2 IT; //!< \var Input tangent
				::Nena::Vector2 OT; //!< \var Output tangent

				Transform::Curve::Key *PK = nullptr; //!< \var Next key
				Transform::Curve::Key *NK = nullptr; //!< \var Prev key
				Transform::Curve *HC = nullptr; //!< \var Host curve

			} TrackKey, AnimationKey;

			//String Name;
			TrackKey::Vector Keys;
			TrackKey *Right = nullptr;
			TrackKey *Left = nullptr;
			String HName;
			String Name;

			::FLOAT ResolveAt(_In_::FLOAT time, _In_::BOOL rec = FALSE);

			union Array
			{
				struct //! \remark allow by-field access
				{
					Transform::Curve *Sx; //!< \var Scale.x animation curve
					Transform::Curve *Sy; //!< \var Scale.x animation curve
					Transform::Curve *Sz; //!< \var Scale.x animation curve
					Transform::Curve *Rx; //!< \var Rotation.x animation curve
					Transform::Curve *Ry; //!< \var Rotation.x animation curve
					Transform::Curve *Rz; //!< \var Rotation.x animation curve
					Transform::Curve *Tx; //!< \var Translation.x animation curve
					Transform::Curve *Ty; //!< \var Translation.x animation curve
					Transform::Curve *Tz; //!< \var Translation.x animation curve
				};
				struct //! \remark allow indexed access
				{
					Transform::Curve::SXArray SRTxyz; //!< \remark access using Curve::Index

					//! \remark Compile-time and run-time functions

					template <CurveIndex _Index> inline Transform::Curve * At() { return SRTxyz[_Index]; }
					inline Transform::Curve * At(CurveIndex i) { return SRTxyz[i]; }

				};

				inline Array() : //! ensure fields are zeros
					Sx(nullptr), Sy(nullptr), Sz(nullptr),
					Rx(nullptr), Ry(nullptr), Rz(nullptr),
					Tx(nullptr), Ty(nullptr), Tz(nullptr)
				{
				}
			};

		} Track, AnimationTrack, Channel;

		::Nena::Vector3 Scale;
		::Nena::Vector3 RotationEA;
		::Nena::Quaternion Rotation;
		::Nena::Quaternion Orientation;
		::Nena::Vector3 Translation;

		Transform::Curve::Array ChannelArray;
		//Transform::Curve::Vector ChannelCollection;
		Transform::Curve::MapToString ChannelMap;

		inline void IsAnimated(::BOOL anim)
		{
			if (anim && ChannelMap.size())
			{
				m_resolveSX = s_resolveSX_Animated;
			}
			else
			{
				m_resolveSX = s_resolveSX_Nonanimated;
			}
		}

		inline ::BOOL IsAnimated()
		{
			return &s_resolveSX_Animated == m_resolveSX;
		}
		 
		inline void ResolveSX(_In_ ::FLOAT time)
		{
			m_resolveSX(this, time);

			/*::Nena::Platform::DebugTrace(
				"## Translation at %f is %f %f %f\n",
				time, Translation.x, Translation.y, Translation.z
				);
			::Nena::Platform::DebugTrace(
				"## Rotation at %f is %f %f %f\n",
				time, RotationEA.x, RotationEA.y, RotationEA.z
				);

			::Nena::Platform::DebugTrace(
				"-------------------------------\n"
				);*/
		}

		Transform();

	private:

		typedef void (CALLBACK * ResolveSXCallback)(Transform *, ::FLOAT);

		ResolveSXCallback m_resolveSX = nullptr;

		inline void ResolveSX_Animated(_In_::FLOAT time)
		{
			ResolveSS_Animated(time);
			ResolveSR_Animated(time);
			ResolveST_Animated(time);


		}

		void ResolveSS_Animated(_In_::FLOAT time);
		void ResolveSR_Animated(_In_::FLOAT time);
		void ResolveST_Animated(_In_::FLOAT time);

		inline static void CALLBACK s_resolveSX_Nonanimated(_In_ Transform *transform, _In_::FLOAT time) 
		{
		}

		inline static void CALLBACK s_resolveSX_Animated(_In_ Transform *transform, _In_::FLOAT time)
		{
			transform->ResolveSX_Animated(time);
		}

	};
}