#pragma once
#include "Job.h"
#include "JobQueue.h"
/*#include <atomic>
#include <queue>
#include <thread>*/


/*
static std::mutex mutex;
void processAnimations(Dawn::Job* InJob)
{
	u32 x = 0;
	for (u32 i = 0; i < 10000; i++)
		x += 1;

	std::lock_guard<std::mutex> lck { mutex };
	DWN_CORE_INFO("Processed Animations {0} on Thread Index: {1}", x, Dawn::JobSystem::ThreadIndex);
}


CJobSystem::Initialize();

	unsigned int ThreadIndex = CJobSystem::ThreadIndex;

	SJob* Root = CJobSystem::CreateJob(&empty_job2);
	for (unsigned int i = 0; i < 500; ++i)
	{
		SJob* Job = CJobSystem::CreateJobAsChild(Root, &empty_job2);
		CJobSystem::Run(Job);
	}

	CJobSystem::Run(Root);
	CJobSystem::Wait(Root);

	//RenderCommand::Packet p = RenderCommand::Create<Draw>(sizeof(Draw));
	//Draw* d = RenderCommand::GetCommand<Draw>(p);
	*/


// THIS JOB SYSTEM IS BASED ON THE SAMPLES PROVIDED BY THE MOLECULAR MUSING 
// BLOG WHICH CAN BE SEEN AT https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/

// Has to be power of two! Because of binary operation in allocate!
#define MAX_JOB_COUNT 4096 

namespace Dawn
{
	class JobSystem
	{
	public:
		static unsigned int ThreadCount;
		thread_local static unsigned int ThreadIndex;

		static void Initialize();
		static void Shutdown();
		static Job* CreateJob(JobFunction InFunction);
		static Job* CreateJobAsChild(Job* InParent, JobFunction InFunction);
		static void AddContinuation(Job* InAncestor, Job* InContinuation);
		static void Run(Job* InJob);
		static Job* ReturnJob();
		static void Wait(const Job* InJob);
		static void Execute(Job* InJob);

		static bool HasJobCompleted(const Job* InJob);
	private:
		static std::vector<std::thread> WorkerThreads;
		static JobQueue WorkerThreadQueues[8];
		static thread_local Job JobAllocator[NUMBER_OF_JOBS];
		static thread_local unsigned int AllocatedJobs;

		static JobQueue* GetWorkerThreadQueue();
		static void TickWorkerThread(int ThreadIndex);
		static bool IsEmpty(Job* InJob);
		static EResult SpawnWorkerThreads();
		static void Finish(Job* InJob);
		static Job* Allocate();
	};
}