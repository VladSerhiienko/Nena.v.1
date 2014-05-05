#ifndef __NENA_BASIC_TIMER_INCLUDED__
#define __NENA_BASIC_TIMER_INCLUDED__

#include <Windows.h>

namespace Nena
{
	namespace Simulation
	{
		class BasicTimer;
	}
}

class Nena::Simulation::BasicTimer
{
	typedef void (BasicTimer::*UpdateDelegate)();
	UpdateDelegate m_UpdateCallback;

	::UINT64 TicksPerSecond64;
	::UINT64 OneSecondTicks64;
	::UINT64 StartupTicks64;
	::UINT64 CurrentTicks64;
	::UINT64 LastTicks64;	

	::BOOL UseFixedDelta;
	::FLOAT FixedDelta;
	::FLOAT Low;

public:

	::INT32 FramesPerSecond;
	::FLOAT TotalElapsed;
	::INT32 FrameCount;
	::FLOAT Elapsed;		

	BasicTimer( ) :
		TicksPerSecond64( 0ui64 ),
		OneSecondTicks64( 0ui64 ),
		UseFixedDelta( false ),
		CurrentTicks64( 0ui64 ),
		StartupTicks64( 0ui64 ),
		FramesPerSecond( 0 ),
		LastTicks64( 0ui64 ),
		FixedDelta( 0.0f ),
		FrameCount( 0 ),
		Elapsed( 0.0f )
	{
		QueryPerformanceFrequency( (::LARGE_INTEGER*) &TicksPerSecond64 );
		QueryPerformanceCounter( (::LARGE_INTEGER*) &CurrentTicks64 );
		OneSecondTicks64 = CurrentTicks64;
		StartupTicks64 = CurrentTicks64;
		SetFixedTimeStep( 0.0f );
	}

	__forceinline void UpdateNonfixedStepImpl( )
	{
		auto const high_elapsed = (::FLOAT) (CurrentTicks64 - StartupTicks64);
		auto const high_delta = (::FLOAT) (CurrentTicks64 - LastTicks64);
		TotalElapsed = (high_elapsed / Low);
		Elapsed = (high_delta / Low);
	}

	__forceinline void UpdateFixedStepImpl( )
	{
		Elapsed = FixedDelta;
		TotalElapsed += Elapsed;
	}

	__forceinline void Update( )
	{
		Low = (::FLOAT) TicksPerSecond64;
		LastTicks64 = CurrentTicks64;

		QueryPerformanceCounter(
			(::LARGE_INTEGER*) &CurrentTicks64
			);
		(this->*m_UpdateCallback)(
			);

		auto const high_sec = (::FLOAT) (CurrentTicks64 - OneSecondTicks64);
		auto const value_sec = (high_sec / Low);

		if ( value_sec < 1.0f ) FrameCount++; else
		{
			OneSecondTicks64 = CurrentTicks64;
			FramesPerSecond = FrameCount;
			FrameCount = 0;
		}
	}

	void SetFixedTimeStep( ::FLOAT step )
	{
		if ( step <= 0.0f )
		{
			FixedDelta = 0.0f;
			UseFixedDelta = false;
			m_UpdateCallback = &BasicTimer::UpdateNonfixedStepImpl;
		}
		else
		{
			UseFixedDelta = true; FixedDelta = step;
			m_UpdateCallback = &BasicTimer::UpdateFixedStepImpl;
		}
	}

	void Reset( )
	{
		FramesPerSecond = 0;
		FrameCount = 0;
		Elapsed = 0.0f;
	}
};

#endif // !__NENA_BASIC_TIMER_INCLUDED__


