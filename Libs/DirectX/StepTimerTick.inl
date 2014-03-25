
// Update timer state, calling the specified Update function the appropriate number of times.
template<typename TUpdate> __forceinline ::HRESULT Nena::Simulation::StepTimer::Tick(const TUpdate& update)
{
	static ::LARGE_INTEGER currentTime = { 0 };
	static ::UINT32 lastFrameCount = 0;
	static ::UINT64 timeDelta = 0;

	// Query the current time.
	if (!QueryPerformanceCounter(&currentTime))
	{
		LastError = E_FAIL;
		return LastError;
	}

	timeDelta = currentTime.QuadPart - m_qpcLastTime.QuadPart;

	m_qpcLastTime = currentTime;
	m_qpcSecondCounter += timeDelta;

	// Clamp excessively large time deltas (e.g. after paused in the debugger).
	if (timeDelta > m_qpcMaxDelta)
	{
		timeDelta = m_qpcMaxDelta;
	}

	// Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
	timeDelta *= TicksPerSecond;
	timeDelta /= m_qpcFrequency.QuadPart;
	lastFrameCount = m_frameCount;

	if (m_isFixedTimeStep)
	{
		// Fixed timestep update logic

		// If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp
		// the clock to exactly match the target value. This prevents tiny and irrelevant errors
		// from accumulating over time. Without this clamping, a game that requested a 60 fps
		// fixed update, running with vsync enabled on a 59.94 NTSC display, would eventually
		// accumulate enough tiny errors that it would drop a frame. It is better to just round 
		// small deviations down to zero to leave things running smoothly.

		if (::abs(static_cast<::INT64>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
		{
			timeDelta = m_targetElapsedTicks;
		}

		m_leftOverTicks += timeDelta;

		while (m_leftOverTicks >= m_targetElapsedTicks)
		{
			m_elapsedTicks = m_targetElapsedTicks;
			m_totalTicks += m_targetElapsedTicks;
			m_leftOverTicks -= m_targetElapsedTicks;
			m_frameCount++;

			update();
		}
	}
	else
	{
		// Variable timestep update logic.
		m_elapsedTicks = timeDelta;
		m_totalTicks += timeDelta;
		m_leftOverTicks = 0;
		m_frameCount++;

		update();
	}

	// Track the current framerate.
	if (m_frameCount != lastFrameCount)
	{
		m_framesThisSecond++;
	}

	if (m_qpcSecondCounter >= static_cast<::UINT64>(m_qpcFrequency.QuadPart))
	{
		m_framesPerSecond = m_framesThisSecond;
		m_framesThisSecond = 0;
		m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
	}

	return S_OK;
}