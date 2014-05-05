#include "app.precompiled.h"

#ifndef __NENA_ANIMATION_INCLUDED__
#define __NENA_ANIMATION_INCLUDED__

namespace Nena
{
	namespace Animation
	{
		enum class EasingFunctionType : ::UINT32
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
		};

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

			EasingFunctionBase (OnFrameCallback on_frame) 
				: OnFrameFunction (on_frame) 
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
				return OnFrameFunction (Params); 
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

		template <EasingFunctionType _FuncTy, EasingFunctionBase::OnFrameCallback _CallbackTy>
		struct EasingFunction : public  EasingFunctionBase { EasingFunction ( ) : EasingFunctionBase (_CallbackTy) {} };

		typedef EasingFunctionBase::Input EasingFunctionInput;
		typedef EasingFunction<EasingFunctionType::kLin, &EasingFunctions::Linear> LinearEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaIn, &EasingFunctions::QuadraticIn> QuadraticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubIn, &EasingFunctions::CubicIn> CubicInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtIn, &EasingFunctions::QuarticIn> QuarticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntIn, &EasingFunctions::QuinticIn> QuinticInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinIn, &EasingFunctions::SinusoidalIn> SinusoidalInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpIn, &EasingFunctions::ExponencialIn> ExponencialInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirIn, &EasingFunctions::CircularIn> CircularInEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaOut, &EasingFunctions::QuadraticOut> QuadraticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubOut, &EasingFunctions::CubicOut> CubicOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtOut, &EasingFunctions::QuarticOut> QuarticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntOut, &EasingFunctions::QuinticOut> QuinticOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinOut, &EasingFunctions::SinusoidalOut> SinusoidalOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpOut, &EasingFunctions::ExponencialOut> ExponencialOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirOut, &EasingFunctions::CircularOut> CircularOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQuaInOut, &EasingFunctions::QuadraticInOut> QuadraticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCubInOut, &EasingFunctions::CubicInOut> CubicInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQrtInOut, &EasingFunctions::QuarticInOut> QuarticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kQntInOut, &EasingFunctions::QuinticInOut> QuinticInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kSinInOut, &EasingFunctions::SinusoidalInOut> SinusoidalInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kExpInOut, &EasingFunctions::ExponencialInOut> ExponencialInOutEasingFunction;
		typedef EasingFunction<EasingFunctionType::kCirInOut, &EasingFunctions::CircularInOut> CircularInOutEasingFunction;

	}
}

#endif // !__NENA_ANIMATION_INCLUDED__
