//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "JobQueue.h"
#include <Windows.h>


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
		COMPILER_BARRIER;
		Bottom = b + 1;
	}

	Job* JobQueue::Steal(void)
	{
		long t = Top;

		COMPILER_BARRIER;

		long b = Bottom;
		if (t < b)
		{
			// non-empty queue
			Job* Job = Jobs[t & MASK];

			// TODO (gb): take a look again at the source material 
			// in order to check why the access violation happens with a lot of tasks
			if (_InterlockedCompareExchange(&Top, t + 1, t) != t)
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
		_InterlockedExchange(&Bottom, b);
		Bottom = b;

		MEMORY_BARRIER;

		long t = Top;
		if (t <= b)
		{
			// non-empty queue
			Job* job = Jobs[b & MASK];
			if (t != b)
			{
				// there's still more than one item left in the queue
				return job;
			}

			// this is the last item in the queue
			if (_InterlockedCompareExchange(&Top, t + 1, t) != t)
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