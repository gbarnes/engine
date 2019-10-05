#pragma once
//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "Job.h"

namespace Dawn
{
	static const unsigned int NUMBER_OF_JOBS = 4096u;
	static const unsigned int MASK = NUMBER_OF_JOBS - 1u;

	class JobQueue
	{
	public:
		JobQueue::JobQueue();
		void Push(Job* InJob);
		Job* Pop();
		Job* Steal();

	private:
		Job* Jobs[NUMBER_OF_JOBS];
		std::atomic<long> Top;
		std::atomic<long> Bottom;
	};
}