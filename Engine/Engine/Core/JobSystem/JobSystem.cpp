//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "JobSystem.h"
#include "../../inc_common.h"
#include <mutex>
#include <thread>

//-----------------------------------------------------------------------------
// Public Static initialization!
//-----------------------------------------------------------------------------
unsigned int CJobSystem::ThreadCount = 1;
thread_local unsigned int CJobSystem::ThreadIndex = 0;

//-----------------------------------------------------------------------------
// Thread specific data
//-----------------------------------------------------------------------------
std::vector<std::thread> CJobSystem::WorkerThreads;
CJobQueue* CJobSystem::WorkerThreadQueues;
std::vector<bool> CJobSystem::IsThreadActive;

//-----------------------------------------------------------------------------
// Job Allocations
//-----------------------------------------------------------------------------
thread_local SJob CJobSystem::JobAllocator[NUMBER_OF_JOBS];
thread_local unsigned int CJobSystem::AllocatedJobs = 0u;

std::mutex cout_mutex;

//-----------------------------------------------------------------------------
// Initializes the JobSystem!
//-----------------------------------------------------------------------------
EResult CJobSystem::Initialize()
{
	ThreadCount = 1;//std::thread::hardware_concurrency();
	WorkerThreadQueues = new CJobQueue[ThreadCount];
	WorkerThreads.resize(ThreadCount - 1);
	IsThreadActive.resize(ThreadCount - 1);
	SpawnWorkerThreads();

	return EResult_OK;
}

EResult CJobSystem::Shutdown()
{
	SafeDelete(WorkerThreadQueues);

	return EResult_OK;
}

//-----------------------------------------------------------------------------
// Beating Heart method from a thread!
//-----------------------------------------------------------------------------
void CJobSystem::TickWorkerThread(int InThreadIndex)
{
	CJobSystem::ThreadIndex = InThreadIndex;
	//std::cout << "Hello World";
	while (IsThreadActive[ThreadIndex - 1] == true)
	{
		SJob* Job = GetJobX();
		if (Job != nullptr)
		{
			Execute(Job);
		}
	}
}

//-----------------------------------------------------------------------------
// Spawns the worker threads and queues!
//-----------------------------------------------------------------------------
EResult CJobSystem::SpawnWorkerThreads()
{
	unsigned int Count = ThreadCount - 1;
	for (unsigned int i = 0; i < Count; i++)
	{
		IsThreadActive[i] = true;
	}

	for (unsigned int i = 0; i < Count; i++)
	{
		WorkerThreads[i] = std::thread(TickWorkerThread, i + 1);
	}

	return EResult_OK;
}


//-----------------------------------------------------------------------------
// Creates a new parent job by a given JobFunction!
//-----------------------------------------------------------------------------
SJob* CJobSystem::CreateJob(JobFunction InFunction)
{
	// the allocation should maybe done differently? Pool Allocator or something
	SJob* Job = Allocate();
	Job->Function = InFunction;
	Job->Parent = nullptr;
	Job->UnfinishedJobs = 1;

	return Job;
}

//-----------------------------------------------------------------------------
// Creates a new job as a child of an existing one. 
//-----------------------------------------------------------------------------
SJob* CJobSystem::CreateJobAsChild(SJob* InParent, JobFunction InFunction)
{
	// increment the count of the unfinished jobs in a atomic fashion
	// in order to prevent race conditions!
	InParent->UnfinishedJobs++;

	// the allocation should maybe done differently? Pool Allocator or something
	SJob* Job = Allocate();
	Job->Function = InFunction;
	Job->Parent = InParent;
	Job->UnfinishedJobs = 1;

	return Job;
}

//-----------------------------------------------------------------------------
// Adds the job to the queue of the current worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::Run(SJob* InJob)
{
	CJobQueue* Queue = GetWorkerThreadQueue();
	Queue->Push(InJob);
}

//-----------------------------------------------------------------------------
// Adds the job to the queue of the current worker thread!
//-----------------------------------------------------------------------------
CJobQueue* CJobSystem::GetWorkerThreadQueue()
{
	return &CJobSystem::WorkerThreadQueues[ThreadIndex];
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::ShutdownWorkerThread(unsigned int InThreadIndex)
{
	if (IsThreadActive.size() < InThreadIndex)
		return;

	IsThreadActive[InThreadIndex] = false;
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
SJob* CJobSystem::GetJobX()
{
	CJobQueue* Queue = GetWorkerThreadQueue();

	SJob* Job = Queue->Pop();
	if (Job == nullptr)
	{
		unsigned int randomIndex = rand() % (ThreadCount - 0 + 1) + 0;

		CJobQueue* StealQueue = &WorkerThreadQueues[randomIndex];
		if (StealQueue == Queue)
		{
			// don't try to steal from ourselves
			std::this_thread::yield();
			return nullptr;
		}

		SJob* StolenJob = StealQueue->Steal();
		
		if (StolenJob == nullptr)
		{
			// we couldn't steal a job from the other queue either, so we just yield our time slice for now
			std::this_thread::yield();
			return nullptr;
		}
		return StolenJob;
	}

	return Job;
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
bool CJobSystem::IsEmpty(SJob* InJob)
{
	return (InJob->UnfinishedJobs == 0);
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::Execute(SJob* InJob)
{
	(InJob->Function)(InJob);
	Finish(InJob);
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::Finish(SJob* InJob)
{
	const unsigned int UnfinishedJobs = InJob->UnfinishedJobs.fetch_sub(1);

	if ((UnfinishedJobs == 0) && (InJob->Parent != nullptr))
	{
		Finish(InJob->Parent);
	}

	// run dependency jobs
	for (std::atomic_uint i = 0; i < InJob->ContinuationCount; ++i)
	{
		Run(InJob->Continuations[i]);
	}
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
SJob* CJobSystem::Allocate()
{
	const unsigned int index = AllocatedJobs++;
	return &JobAllocator[index & (MAX_JOB_COUNT - 1u)];
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
bool CJobSystem::HasJobCompleted(const SJob* InJob)
{
	return (InJob->UnfinishedJobs == 0);
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::AddContinuation(SJob* InAncestor, SJob* InContinuation)
{
	const int32_t Count = InAncestor->ContinuationCount.fetch_add(1);
	InAncestor->Continuations[Count - 1] = InContinuation;
}

//-----------------------------------------------------------------------------
// Closes a given worker thread!
//-----------------------------------------------------------------------------
void CJobSystem::Wait(const SJob* InJob)
{
	// wait until the job has completed. in the meantime, work on any other job.
	while (!HasJobCompleted(InJob))
	{
		SJob* NextJob = GetJobX();
		if (NextJob != nullptr)
		{
			Execute(NextJob);
		}
	}
}