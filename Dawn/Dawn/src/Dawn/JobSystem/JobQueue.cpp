
//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "JobQueue.h"
#include <Windows.h>
#include <iostream>


#define COMPILER_BARRIER _ReadWriteBarrier()
#define MEMORY_BARRIER MemoryBarrier()

namespace Dawn
{
	JobQueue::JobQueue()
	{
		Bottom = 0;
		Top = 0;
	}

	void JobQueue::Push(Job* InJob)
	{
		long b = Bottom;
		Jobs[b & MASK] = InJob;
		Bottom = b + 1;
	}

	Job* JobQueue::Steal(void)
	{
		long t = Top;
		long b = Bottom;

		if (t < b)
		{
			// non-empty queue
			Job* Job = Jobs[t & MASK];

			if (!Top.compare_exchange_strong(t, t + 1))
			{
				// a concurrent steal or pop operation removed an element from the deque in the meantime.
				return nullptr;
			}

			return Job;
		}
		else
		{
			// empty queue
			return nullptr;
		}
	}


	Job* JobQueue::Pop(void)
	{
		long b = Bottom - 1;
		Bottom = b;

		long t = Top;
		if (t <= b)
		{
			std::cout << "Found a job to process!" << std::endl;
			// non-empty queue
			Job* job = Jobs[b & MASK];
			if (t != b)
			{
				// there's still more than one item left in the queue
				return job;
			}

			// this is the last item in the queue
			if (!Top.compare_exchange_strong(t, t + 1))
			{
				// failed race against steal operation
				job = nullptr;
			}

			Bottom = t + 1;
			return job;
		}
		else
		{
			// deque was already empty
			Bottom = t;
			return nullptr;
		}
	}
}