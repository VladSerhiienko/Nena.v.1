#include "DirectXPch.h"
#include "StepTimer.h"

Nena::Simulation::StepTimer::StepTimer() :
	LastError(S_OK),
	m_elapsedTicks(0),
	m_totalTicks(0),
	m_leftOverTicks(0),
	m_frameCount(0),
	m_framesPerSecond(0),
	m_framesThisSecond(0),
	m_qpcSecondCounter(0),
	m_isFixedTimeStep(false),
	m_targetElapsedTicks(TicksPerSecond / 60)
{
	if (!QueryPerformanceFrequency(&m_qpcFrequency))
	{
		LastError = E_FAIL;
		return;
	}

	if (!QueryPerformanceCounter(&m_qpcLastTime))
	{
		LastError = E_FAIL;
		return;
	}

	// Initialize max delta to 1/10 of a second.
	m_qpcMaxDelta = m_qpcFrequency.QuadPart / 10;
}

::HRESULT Nena::Simulation::StepTimer::ResetElapsedTime()
{
	if (!QueryPerformanceCounter(&m_qpcLastTime))
	{
		LastError = E_FAIL;
		return LastError;
	}

	m_leftOverTicks = 0;
	m_framesPerSecond = 0;
	m_framesThisSecond = 0;
	m_qpcSecondCounter = 0;

	return S_OK;
}