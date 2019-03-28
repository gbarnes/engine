#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "Job.h"
#include "JobQueue.h"
#include <atomic>
#include <queue>
#include <thread>

#include "inc_common.h"

// Has to be power of two! Because of binary operation in allocate!
#define MAX_JOB_COUNT 4096 

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  
	//-----------------------------------------------------------------------------
	class JobSystem
	{
	public:
		static unsigned int ThreadCount;
		thread_local static unsigned int ThreadIndex;

		static EResult Initialize();
		static EResult Shutdown();
		static Job* CreateJob(JobFunction InFunction);
		static Job* CreateJobAsChild(Job* InParent, JobFunction InFunction);
		static void AddContinuation(Job* InAncestor, Job* InContinuation);
		static void Run(Job* InJob);
		static Job* GetJobX();
		static void Wait(const Job* InJob);
		static void Execute(Job* InJob);
		static void ShutdownWorkerThread(unsigned int InThreadIndex);

	private:
		static std::vector<std::thread> WorkerThreads;
		static JobQueue* WorkerThreadQueues;
		static std::vector<bool> IsThreadActive;
		static thread_local Job JobAllocator[NUMBER_OF_JOBS];
		static thread_local unsigned int AllocatedJobs;

		static JobQueue* GetWorkerThreadQueue();
		static void TickWorkerThread(int ThreadIndex);
		static bool IsEmpty(Job* InJob);
		static EResult SpawnWorkerThreads();
		static void Finish(Job* InJob);
		static Job* Allocate();
		static bool HasJobCompleted(const Job* InJob);
	};
}