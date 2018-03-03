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
#include "../../inc_common.h"

// Has to be power of two! Because of binary operation in allocate!
#define MAX_JOB_COUNT 4096 


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  
//-----------------------------------------------------------------------------
class CJobSystem
{
public:
	static unsigned int ThreadCount;
	thread_local static unsigned int ThreadIndex;

	static EResult Initialize();
	static EResult Shutdown();
	static SJob* CreateJob(JobFunction InFunction);
	static SJob* CreateJobAsChild(SJob* InParent, JobFunction InFunction);
	static void AddContinuation(SJob* InAncestor, SJob* InContinuation);
	static void Run(SJob* InJob);
	static SJob* GetJobX();
	static void Wait(const SJob* InJob);
	static void Execute(SJob* InJob);
	static void ShutdownWorkerThread(unsigned int InThreadIndex);

private:
	static std::vector<std::thread> WorkerThreads;
	static CJobQueue* WorkerThreadQueues;
	static std::vector<bool> IsThreadActive;
    static thread_local SJob JobAllocator[NUMBER_OF_JOBS];
	static thread_local unsigned int AllocatedJobs;

	static CJobQueue* GetWorkerThreadQueue();
	static void TickWorkerThread(int ThreadIndex);
	static bool IsEmpty(SJob* InJob);
	static EResult SpawnWorkerThreads();
	static void Finish(SJob* InJob);
	static SJob* Allocate();
	static bool HasJobCompleted(const SJob* InJob);
};