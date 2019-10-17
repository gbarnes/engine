#include "Timer.h"

namespace Dawn
{
	float Timer::CyclesPerSecond = 0.0f;
	LARGE_INTEGER Time::ClockFrequency;
	float Time::TargetUpdateRate = 1.0f / 60.0f;
	float Time::TargetPhysicsUpdateRate = 1.0f / 30.0f;

	void InitTime(Time& InTime, float InTargetUpdateRate)
	{
		QueryPerformanceFrequency(&Time::ClockFrequency);
		QueryPerformanceCounter(&InTime.TimeFrameBegin);
		Time::TargetUpdateRate = InTargetUpdateRate;
		InTime.bIsInitialized = true;
	}

	void StepTime(Time& InTime)
	{
		if (!InTime.bIsInitialized)
			return;

		if (InTime.bIsPaused)
		{
			QueryPerformanceCounter(&InTime.TimeFrameEnd);
			InTime.FrameDeltaTime = 0.0f;
			InTime.TimeFrameBegin = InTime.TimeFrameEnd;
			return;
		}

		QueryPerformanceCounter(&InTime.TimeFrameEnd);
		InTime.FrameDeltaTime = (((float)(InTime.TimeFrameEnd.QuadPart - InTime.TimeFrameBegin.QuadPart) / (float)Time::ClockFrequency.QuadPart));
		InTime.TimeFrameBegin = InTime.TimeFrameEnd;

		// if we run too slow just use the target update rate!
		if (InTime.FrameDeltaTime > 1.0f / 10.0f)
			InTime.FrameDeltaTime = Time::TargetUpdateRate;
	}
}