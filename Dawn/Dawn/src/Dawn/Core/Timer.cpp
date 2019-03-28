#include "Timer.h"

namespace Dawn 
{
	STime Timer::Time;

	Timer::Timer()
		: DeltaTime(0), TotalTime(0)
	{
		T0 = std::chrono::high_resolution_clock::now();
	}

	void Timer::Tick()
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		DeltaTime = t1 - T0;
		TotalTime += DeltaTime;
		T0 = t1;

		Time.InternalDeltaTime = DeltaTime.count();
		Time.InternalTotalTime = TotalTime.count();
	}

	void Timer::Reset()
	{
		T0 = std::chrono::high_resolution_clock::now();
		DeltaTime = std::chrono::high_resolution_clock::duration();
		TotalTime = std::chrono::high_resolution_clock::duration();
	}

	STime& Timer::GetTime()
	{
		return Time;
	}

	double STime::GetDeltaNanoseconds() const
	{
		return InternalDeltaTime * 1.0;
	}
	double STime::GetDeltaMicroseconds() const
	{
		return InternalDeltaTime * 1e-3;
	}

	double STime::GetDeltaMilliseconds() const
	{
		return InternalDeltaTime * 1e-6;
	}

	double STime::GetDeltaSeconds() const
	{
		return InternalDeltaTime * 1e-9;
	}

	double STime::GetTotalNanoseconds() const
	{
		return InternalTotalTime * 1.0;
	}

	double STime::GetTotalMicroseconds() const
	{
		return InternalTotalTime * 1e-3;
	}

	double STime::GetTotalMilliseconds() const
	{
		return InternalTotalTime * 1e-6;
	}
	
	double STime::GetTotalSeconds() const
	{
		return InternalTotalTime * 1e-9;
	}

}