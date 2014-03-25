#include "DirectXPch.h"

#ifndef __NENA_DIRECTX_STEPTIMER_H__
#define __NENA_DIRECTX_STEPTIMER_H__

#define __NENA_DIRECTX_Steptimer_inlining __forceinline

namespace Nena
{
	namespace Simulation
	{
		// Helper class for animation and simulation timing.
		struct StepTimer
		{
			// Constructor for timer class.
			Nena::Simulation::StepTimer::StepTimer();

			// After an intentional timing discontinuity (for instance a blocking IO operation)
			// call this to avoid having the fixed timestep logic attempt a set of catch-up 
			// Update calls.

			::HRESULT Nena::Simulation::StepTimer::ResetElapsedTime();

			/// Inlined class members:

			// Get elapsed time since the previous Update call.
			__forceinline ::UINT64 Nena::Simulation::StepTimer::GetElapsedTicks( ) const;
			// Get elapsed time since the previous Update call.
			__forceinline ::DOUBLE Nena::Simulation::StepTimer::GetElapsedSeconds() const;

			// Get total time since the start of the program.
			__forceinline ::UINT64 Nena::Simulation::StepTimer::GetTotalTicks( ) const;
			// Get total time since the start of the program.
			__forceinline ::DOUBLE Nena::Simulation::StepTimer::GetTotalSeconds() const;

			// Get total number of updates since start of the program.
			__forceinline ::UINT32 Nena::Simulation::StepTimer::GetFrameCount( ) const;

			// Get the current framerate.
			__forceinline ::UINT32 Nena::Simulation::StepTimer::GetFramesPerSecond( ) const;

			// Set whether to use fixed or variable timestep mode.
			__forceinline void Nena::Simulation::StepTimer::SetFixedTimeStep(_In_ ::BOOL isFixedTimestep);

			// Set how often to call Update when in fixed timestep mode.
			__forceinline void Nena::Simulation::StepTimer::SetTargetElapsedTicks(_In_ ::UINT64 targetElapsed);
			// Set how often to call Update when in fixed timestep mode.
			__forceinline void Nena::Simulation::StepTimer::SetTargetElapsedSeconds(_In_::DOUBLE targetElapsed);

			// Integer format represents time using 10,000,000 ticks per second.
			static const ::UINT64 TicksPerSecond = 10000000;

			__forceinline static ::DOUBLE Nena::Simulation::StepTimer::TicksToSeconds(_In_ ::UINT64 ticks);
			__forceinline static ::UINT64 Nena::Simulation::StepTimer::SecondsToTicks(_In_::DOUBLE seconds);

			// Update timer state, calling the specified Update function the appropriate number of times.
			template<typename TUpdate> __forceinline ::HRESULT Tick(_In_ const TUpdate& update);

		private:

			// Source timing data uses QPC units.
			::LARGE_INTEGER m_qpcFrequency;
			::LARGE_INTEGER m_qpcLastTime;
			::UINT64 m_qpcMaxDelta;

			// Derived timing data uses a canonical tick format.
			::UINT64 m_elapsedTicks;
			::UINT64 m_totalTicks;
			::UINT64 m_leftOverTicks;

			// Members for tracking the framerate.
			::UINT32 m_frameCount;
			::UINT32 m_framesPerSecond;
			::UINT32 m_framesThisSecond;
			::UINT64 m_qpcSecondCounter;

			// Members for configuring fixed timestep mode.
			::UINT64 m_targetElapsedTicks;
			::BOOL m_isFixedTimeStep;

		public:

			// Notifies about errors (mainly, failed to query performance frequency).
			::HRESULT LastError;
		};

#include "StepTimerTick.inl"
#include "StepTimerAccessors.inl"

	}
}

#endif // !__NENA_DIRECTX_STEPTIMER_H__
