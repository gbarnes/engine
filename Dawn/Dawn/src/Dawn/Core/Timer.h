
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

		u128 GetTimeCycles() const
		{
			return TimeCycles;
		}

		float CalculateDeltaSeconds(const Timer& Other)
		{
			u128 dt = TimeCycles - Other.TimeCycles;
			return CyclesToSeconds(dt);
		}

		void Update(float InRealDtSeconds)
		{
			if (!bIsPaused)
			{
				u128 dtScaledCycles = SecondsToCycles(InRealDtSeconds * TimeScale);
				TimeCycles += dtScaledCycles;
			}
		}

	private:
		static float CyclesPerSecond;

		u128 TimeCycles;
		float TimeScale;
		bool bIsPaused;

		static inline u128 SecondsToCycles(float TimeSeconds)
		{
			return (u128)(TimeSeconds * CyclesPerSecond);
		}

		// Only use this method to convert small durations into seconds!
		static inline float CyclesToSeconds(u128 TimeCycles)
		{
			return (float)TimeCycles / CyclesPerSecond;
		}

	};
}