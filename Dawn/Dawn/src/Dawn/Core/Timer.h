
#pragma once
#include "inc_core.h"
#include <chrono>

namespace Dawn
{
	//
	// This class is based on the example from
	// the boock Game Engine Architecture from Jason Gregory.
	//
	class DAWN_API Timer
	{


	public:

		static void Init()
		{
			LARGE_INTEGER Frequency;
			QueryPerformanceFrequency(&Frequency);
			CyclesPerSecond = (float)Frequency.QuadPart;
		}

		explicit Timer(float StartTimeSeconds = 0.0f)
			: TimeCycles(SecondsToCycles(StartTimeSeconds)),
			TimeScale(1.0f),
			bIsPaused(false)
		{

		}

		u64 GetTimeCycles() const
		{
			return TimeCycles;
		}

		float CalculateDeltaSeconds(const Timer& Other)
		{
			u64 dt = TimeCycles - Other.TimeCycles;
			return CyclesToSeconds(dt);
		}

		void Update(float InRealDtSeconds)
		{
			if (!bIsPaused)
			{
				u64 dtScaledCycles = SecondsToCycles(InRealDtSeconds * TimeScale);
				TimeCycles += dtScaledCycles;
			}
		}

	private:
		static float CyclesPerSecond;

		u64 TimeCycles;
		float TimeScale;
		bool bIsPaused;

		static inline u64 SecondsToCycles(float TimeSeconds)
		{
			return (u64)(TimeSeconds * CyclesPerSecond);
		}

		// Only use this method to convert small durations into seconds!
		static inline float CyclesToSeconds(u64 TimeCycles)
		{
			return (float)TimeCycles / CyclesPerSecond;
		}

	};


	struct DAWN_API Time
	{
		static LARGE_INTEGER ClockFrequency;
		static float TargetUpdateRate;
		static float TargetPhysicsUpdateRate;

		float AlignedUpdateDeltaTime = 0.0f;
		float AlignedPhysicsDeltaTime = 0.0f;
		float FrameDeltaTime = TargetUpdateRate;
		float TimeScale = 1.0f;
		bool bIsInitialized = false;
		bool bIsPaused = false;
		u64 FrameCount;

		LARGE_INTEGER TimeFrameBegin;
		LARGE_INTEGER TimeFrameEnd;
	};

	DAWN_API void InitTime(Time& InTime, float InTargetUpdateRate = 1.0f/60.0f);
	DAWN_API void StepTime(Time& InTime);
}