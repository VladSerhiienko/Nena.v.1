#pragma once

#include "app.precompiled.h"

#ifndef __NENA_ANIMATION_INCLUDED__
#define __NENA_ANIMATION_INCLUDED__

namespace Nena
{
	namespace Animation
	{
		struct EasingFunctionBase
		{
			typedef ::FLOAT Real;
			typedef ::BOOL Boolean;

			typedef union Input
			{
				struct { EasingFunctionBase::Real Offset, Distance, Total, Elapsed; };
				struct { EasingFunctionBase::Real b, c, d, t; };
			} Input;

			typedef EasingFunctionBase::Real( XM_CALLCONV * OnFrameCallback )(
				_In_ EasingFunctionBase::Input
				);

			EasingFunctionBase( OnFrameCallback on_frame )
				: OnFrameFunction( on_frame )
			{
			}

			EasingFunctionBase::Input Params;
			EasingFunctionBase::Boolean Saturated = FALSE;
			EasingFunctionBase::OnFrameCallback OnFrameFunction;

			__forceinline void Reset( )
			{
				Params.Elapsed = 0.0f;
				Saturated = FALSE;
			}
			__forceinline EasingFunctionBase::Real OnFrame( )
			{
				return OnFrameFunction( Params );
			}
			__forceinline void operator += (EasingFunctionBase::Real tick_time)
			{
				if ( !Saturated )
				{
					if ( Params.Elapsed < Params.Total )
						Params.Elapsed += tick_time;
					else
						Params.Elapsed = Params.Total,
						Saturated = TRUE;
				}
			}
			__forceinline void operator >>= (EasingFunctionBase::Real tick_time)
			{
				Params.Elapsed += tick_time;
			}

		};

		typedef enum class EasingStage
		{
			kSuspending,	//! @brief moving to the left away, unresponsive
			kSuspended,		//! @brief not visible, unresponsive
			kResuming,		//! @brief becoming visible, unresponsive
			kResumed,		//! @brief visible, responsive
			kMax,			//! @brief easing stages count
			kDefault = kSuspended
		} Stage;

		typedef enum class EasingFunctionType : ::UINT32
		{
			kLin,		//! @brief no easing, no acceleration 
			kQuaIn,		//! @brief accelerating from zero velocity 
			kQuaOut,	//! @brief decelerating to zero velocity 
			kQuaInOut,	//! @brief acceleration until halfway, then deceleration
			kCubIn,		//! @brief accelerating from zero velocity 
			kCubOut,	//! @brief decelerating to zero velocity
			kCubInOut,	//! @brief acceleration until halfway, then deceleration
			kQrtIn,		//! @brief accelerating from zero velocity
			kQrtOut,	//! @brief decelerating to zero velocity
			kQrtInOut,	//! @brief acceleration until halfway, then deceleration
			kQntIn,		//! @brief accelerating from zero velocity
			kQntOut,	//! @brief decelerating to zero velocity
			kQntInOut,	//! @brief acceleration until halfway, then deceleration
			kSinIn,		//! @brief accelerating from zero velocity
			kSinOut,	//! @brief decelerating to zero velocity
			kSinInOut,	//! @brief acceleration until halfway, then deceleration
			kExpIn,		//! @brief accelerating from zero velocity
			kExpOut,	//! @brief decelerating to zero velocity
			kExpInOut,	//! @brief acceleration until halfway, then deceleration
			kCirIn,		//! @brief accelerating from zero velocity
			kCirOut,	//! @brief decelerating to zero velocity
			kCirInOut,	//! @brief acceleration until halfway, then deceleration

			kLinIn = kLin,
			kLinOut = kLin,
			kLinInOut = kLin,
			kLinPair = kLinIn,
			kQuaPair = kQuaIn,
			kCubPair = kCubIn,
			kQrtPair = kQrtIn,
			kQntPair = kQntIn,
			kSinPair = kSinIn,
			kExpPair = kExpIn,
			kCirPair = kCirIn,
			kMax = kCirInOut,

		} EasingFunctionType;

		template< typename T > struct UnitSteppableEasingFunctionType { enum { Value = FALSE }; };
		template<> struct UnitSteppableEasingFunctionType< EasingFunctionType > { enum { Value = TRUE }; };
		template< typename T > typename std::enable_if< UnitSteppableEasingFunctionType< T >::Value, T >::type operator++(T value)
		{
			return T( value + 1 );
		}
		template< typename T > typename std::enable_if< UnitSteppableEasingFunctionType< T >::Value, T >::type operator+(T value, int inc)
		{
			return T( value + inc );
		}
		template< typename T > typename std::enable_if< UnitSteppableEasingFunctionType< T >::Value, T >::type operator+(T value, unsigned inc)
		{
			return T( value + inc );
		}


		struct EasingFunctions
		{
#pragma region Easing functions implementation
			__forceinline static EasingFunctionBase::Real XM_CALLCONV Linear( _In_ EasingFunctionBase::Input i )
			{
				return i.c*i.t / i.d + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuadraticIn( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return i.c * i.t * i.t + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuadraticOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return -i.c * i.t * (i.t - 2.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuadraticInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return i.c / 2.0f * i.t * i.t + i.b;
				i.t--;
				return -i.c / 2.0f * (i.t * (i.t - 2.0f) - 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CubicIn( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return i.c*i.t * i.t * i.t + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CubicOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				i.t--;
				return i.c*(i.t * i.t * i.t + 1.0f) + i.b;

			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CubicInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return i.c / 2.0f * i.t * i.t * i.t + i.b;
				i.t -= 2.0f;
				return i.c / 2.0f * (i.t * i.t * i.t + 2.0f) + i.b;

			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuarticIn( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return i.c*i.t * i.t * i.t * i.t + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuarticOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				i.t--;
				return -i.c * (i.t * i.t * i.t * i.t - 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuarticInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return i.c / 2.0f * i.t * i.t * i.t * i.t + i.b;
				i.t -= 2.0f;
				return -i.c / 2.0f * (i.t * i.t * i.t * i.t - 2.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuinticIn( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return i.c*i.t * i.t * i.t * i.t * i.t + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuinticOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				i.t--;
				return -i.c * (i.t * i.t * i.t * i.t * i.t + 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV QuinticInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return i.c / 2.0f * i.t * i.t * i.t * i.t * i.t + i.b;
				i.t -= 2.0f;
				return i.c / 2.0f * (i.t * i.t * i.t * i.t * i.t + 2.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV SinusoidalIn( _In_ EasingFunctionBase::Input i )
			{
				return -i.c * cosf( i.t / i.d * (XM_PI / 2.0f) ) + i.c + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV SinusoidalOut( _In_ EasingFunctionBase::Input i )
			{
				return i.c * sinf( i.t / i.d * (XM_PI / 2.0f) ) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV SinusoidalInOut( _In_ EasingFunctionBase::Input i )
			{
				return -i.c / 2.0f * (cosf( XM_PI*i.t / i.d ) - 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV ExponencialIn( _In_ EasingFunctionBase::Input i )
			{
				return i.c * powf( 2.0f, 10.0f * (i.t / i.d - 1.0f) ) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV ExponencialOut( _In_ EasingFunctionBase::Input i )
			{
				return i.c * (-powf( 2.0f, -10.0f * i.t / i.d ) + 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV ExponencialInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return i.c / 2.0f * powf( 2.0f, 10.0f * (i.t - 1.0f) ) + i.b;
				i.t--;
				return i.c / 2.0f * (-powf( 2.0f, -10.0f * i.t ) + 2.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CircularIn( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				return -i.c * (sqrtf( 1.0f - i.t * i.t ) - 1.0f) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CircularOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d;
				i.t--;
				return i.c * sqrtf( 1.0f - i.t * i.t ) + i.b;
			}
			__forceinline static EasingFunctionBase::Real XM_CALLCONV CircularInOut( _In_ EasingFunctionBase::Input i )
			{
				i.t /= i.d / 2.0f;
				if ( i.t < 1.0f ) return -i.c / 2.0f * (sqrtf( 1.0f - i.t * i.t ) - 1.0f) + i.b;
				i.t -= 2.0f;
				return i.c / 2.0f * (sqrtf( 1.0f - i.t * i.t ) + 1.0f) + i.b;
			}
#pragma endregion
		};

		template <EasingFunctionType _FuncTy> struct EasingFunction;

		typedef EasingFunctionBase::Input EasingFunctionInput;
		template <> struct EasingFunction<EasingFunctionType::kLin> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::Linear ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQuaIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuadraticIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCubIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CubicIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQrtIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuarticIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQntIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuinticIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kSinIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::SinusoidalIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kExpIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::ExponencialIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCirIn> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CircularIn ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQuaOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuadraticOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCubOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CubicOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQrtOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuarticOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQntOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuinticOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kSinOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::SinusoidalOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kExpOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::ExponencialOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCirOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CircularOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQuaInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuadraticInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCubInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CubicInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQrtInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuarticInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kQntInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::QuinticInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kSinInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::SinusoidalInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kExpInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::ExponencialInOut ) {} };
		template <> struct EasingFunction<EasingFunctionType::kCirInOut> : public EasingFunctionBase { EasingFunction( ) : EasingFunctionBase( &EasingFunctions::CircularInOut ) {} };

		typedef EasingFunction<EasingFunctionType::kLin> LinearEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaIn> QuadraticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubIn> CubicInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtIn> QuarticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntIn> QuinticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinIn> SinusoidalInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpIn> ExponencialInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirIn> CircularInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaOut> QuadraticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubOut> CubicOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtOut> QuarticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntOut> QuinticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinOut> SinusoidalOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpOut> ExponencialOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirOut> CircularOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaInOut> QuadraticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubInOut> CubicInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtInOut> QuarticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntInOut> QuinticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinInOut> SinusoidalInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpInOut> ExponencialInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirInOut> CircularInOutEasingFunction;

		template <EasingFunctionType _FuncResTy, EasingFunctionType _FuncSusTy> struct EasingFunctionPair
		{
			EasingStage CurrentStage = EasingStage::kDefault;
			EasingFunctionBase::Real LastValue = 0.0f;
			EasingFunction<_FuncResTy> SuspendingValue;
			EasingFunction<_FuncSusTy> ResumingValue;

			inline EasingFunctionBase::Real OnFrame( )
			{
				switch ( CurrentStage )
				{
				case EasingStage::kSuspending:
					return SuspendingValue.OnFrame( );
				case EasingStage::kResuming:
					return ResumingValue.OnFrame( );
				}

				return LastValue;
			}
			inline void operator += (EasingFunctionBase::Real tick)
			{
				switch ( CurrentStage )
				{
				case EasingStage::kResuming:
					ResumingValue += tick;
					if ( ResumingValue.Saturated )
						CurrentStage = EasingStage::kResumed,
						LastValue = ResumingValue.OnFrame( ),
						SuspendingValue.Reset( );
					return;
				case EasingStage::kSuspending:
					SuspendingValue += tick;
					if ( SuspendingValue.Saturated )
						CurrentStage = EasingStage::kSuspended,
						LastValue = SuspendingValue.OnFrame(),
						ResumingValue.Reset( );
					return;
				}
			}
			inline EasingFunctionBase::Boolean IsResponsive( )
			{
				return SuspendingValue.Saturated || ResumingValue.Saturated;
			}
			inline void SetTotal( EasingFunctionBase::Real value )
			{
				ResumingValue.Params.Total = value;
				SuspendingValue.Params.Total = value * 0.5f;
			}
			inline void IsSaturated( )
			{
				switch ( CurrentStage )
				{
				case EasingStage::kSuspending:
				case EasingStage::kResuming:
					return FALSE;
				default:
					return TRUE;
				}
			}
			inline void Reset( )
			{
				SuspendingValue.Reset( );
				ResumingValue.Reset( );
			}
			inline void Resume( )
			{
				Reset( ); CurrentStage = EasingStage::kResuming;
			}
			inline void Suspend( )
			{
				Reset( ); CurrentStage = EasingStage::kSuspending;
			}

		};

		typedef EasingFunctionPair<EasingFunctionType::kLinIn, EasingFunctionType::kLinOut> LinearEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kQuaIn, EasingFunctionType::kQuaOut> QuadricEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kCubIn, EasingFunctionType::kCubOut> CubicEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kQrtIn, EasingFunctionType::kQrtOut> QuarticEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kQntIn, EasingFunctionType::kQntOut> QuinticEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kSinIn, EasingFunctionType::kSinOut> SinusoidalEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kExpIn, EasingFunctionType::kExpOut> ExponencialEasingFunctionPair;
		typedef EasingFunctionPair<EasingFunctionType::kCirIn, EasingFunctionType::kCirOut> CircularEasingFunctionPair;
	}
}

#endif // !__NENA_ANIMATION_INCLUDED__
