#pragma once
#include "inc_core.h"
#include <chrono>

namespace Dawn
{
	struct STime
	{
		friend class Timer;
	private:
		long long InternalDeltaTime;
		long long InternalTotalTime;
	public:
		
		double STime::GetDeltaNanoseconds() const;
		double STime::GetDeltaMicroseconds() const;
		double STime::GetDeltaMilliseconds() const;
		double STime::GetDeltaSeconds() const;

		double STime::GetTotalNanoseconds() const;
		double STime::GetTotalMicroseconds() const;
		double STime::GetTotalMilliseconds() const;
		double STime::GetTotalSeconds() const;
	};

	class DAWN_API Timer
	{
	public:
		Timer();

		// Tick the high resolution clock.
		// Tick the clock before reading the delta time for the first time.
		// Only tick the clock once per frame.
		// Use the Get* functions to return the elapsed time between ticks.
		void Tick();

		// Reset the clock.
		void Reset();

		static STime& GetTime();
	private:

		static STime Time;

		// Initial time point.
		std::chrono::high_resolution_clock::time_point T0;
		// Time since last tick.
		std::chrono::high_resolution_clock::duration DeltaTime;
		std::chrono::high_resolution_clock::duration TotalTime;
	};
}