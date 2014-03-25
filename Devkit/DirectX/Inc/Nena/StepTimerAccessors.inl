#include "StepTimer.h"

// Get elapsed time since the previous Update call.
__forceinline ::UINT64 Nena::Simulation::StepTimer::GetElapsedTicks() const
{
	return m_elapsedTicks;
}
__forceinline ::DOUBLE Nena::Simulation::StepTimer::GetElapsedSeconds() const
{
	return TicksToSeconds(m_elapsedTicks);
}

// Get total time since the start of the program.
__forceinline ::UINT64 Nena::Simulation::StepTimer::GetTotalTicks() const
{
	return m_totalTicks;
}
__forceinline ::DOUBLE Nena::Simulation::StepTimer::GetTotalSeconds() const
{
	return TicksToSeconds(m_totalTicks);
}

// Get total number of updates since start of the program.
__forceinline ::UINT32 Nena::Simulation::StepTimer::GetFrameCount() const
{
	return m_frameCount;
}

// Get the current framerate.
__forceinline ::UINT32 Nena::Simulation::StepTimer::GetFramesPerSecond() const
{
	return m_framesPerSecond;
}

// Set whether to use fixed or variable timestep mode.
__forceinline void Nena::Simulation::StepTimer::SetFixedTimeStep(::BOOL isFixedTimestep)
{
	m_isFixedTimeStep = isFixedTimestep;
}

// Set how often to call Update when in fixed timestep mode.
__forceinline void Nena::Simulation::StepTimer::SetTargetElapsedTicks(::UINT64 targetElapsed)
{
	m_targetElapsedTicks = targetElapsed;
}
__forceinline void Nena::Simulation::StepTimer::SetTargetElapsedSeconds(::DOUBLE targetElapsed)
{
	m_targetElapsedTicks = SecondsToTicks(targetElapsed);
}

__forceinline ::DOUBLE Nena::Simulation::StepTimer::TicksToSeconds(::UINT64 ticks)
{
	return static_cast<::DOUBLE>(ticks) / TicksPerSecond;
}
__forceinline ::UINT64 Nena::Simulation::StepTimer::SecondsToTicks(::DOUBLE seconds)
{
	return static_cast<::UINT64>(seconds * TicksPerSecond);
}