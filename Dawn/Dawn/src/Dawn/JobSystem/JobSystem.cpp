
//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "JobSystem.h"
#include <mutex>
#include <thread>

namespace Dawn
{

	//-----------------------------------------------------------------------------
	// Public Static initialization!
	//-----------------------------------------------------------------------------
	unsigned int JobSystem::ThreadCount = 1;
	thread_local unsigned int JobSystem::ThreadIndex = 0;

	//-----------------------------------------------------------------------------
	// Thread specific data
	//-----------------------------------------------------------------------------
	std::vector<std::thread> JobSystem::WorkerThreads;
	JobQueue* JobSystem::WorkerThreadQueues;
	std::vector<bool> JobSystem::IsThreadActive;

	//-----------------------------------------------------------------------------
	// Job Allocations
	//-----------------------------------------------------------------------------
	thread_local Job JobSystem::JobAllocator[NUMBER_OF_JOBS];
	thread_local unsigned int JobSystem::AllocatedJobs = 0u;

	std::mutex cout_mutex;

	//-----------------------------------------------------------------------------
	// Initializes the JobSystem!
	//-----------------------------------------------------------------------------
	EResult JobSystem::Initialize()
	{
		ThreadCount = std::thread::hardware_concurrency();
		WorkerThreadQueues = new JobQueue[ThreadCount];
		WorkerThreads.resize(ThreadCount - 1);
		IsThreadActive.resize(ThreadCount - 1);
		//SpawnWorkerThreads();

		return EResult_OK;
	}

	EResult JobSystem::Shutdown()
	{
		SafeDelete(WorkerThreadQueues);

		return EResult_OK;
	}

	//-----------------------------------------------------------------------------
	// Beating Heart method from a thread!
	//-----------------------------------------------------------------------------
	void JobSystem::TickWorkerThread(int InThreadIndex)
	{
		JobSystem::ThreadIndex = InThreadIndex;
		//std::cout << "Hello World";
		while (IsThreadActive[ThreadIndex - 1] == true)
		{
			Job* Job = GetJobX();
			if (Job != nullptr)
			{
				Execute(Job);
			}
		}
	}

	//-----------------------------------------------------------------------------
	// Spawns the worker threads and queues!
	//-----------------------------------------------------------------------------
	EResult JobSystem::SpawnWorkerThreads()
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
	Job* JobSystem::CreateJob(JobFunction InFunction)
	{
		// the allocation should maybe done differently? Pool Allocator or something
		Job* Job = Allocate();
		Job->Function = InFunction;
		Job->Parent = nullptr;
		Job->UnfinishedJobs = 1;

		return Job;
	}

	//-----------------------------------------------------------------------------
	// Creates a new job as a child of an existing one. 
	//-----------------------------------------------------------------------------
	Job* JobSystem::CreateJobAsChild(Job* InParent, JobFunction InFunction)
	{
		// increment the count of the unfinished jobs in a atomic fashion
		// in order to prevent race conditions!
		InParent->UnfinishedJobs++;

		// the allocation should maybe done differently? Pool Allocator or something
		Job* Job = Allocate();
		Job->Function = InFunction;
		Job->Parent = InParent;
		Job->UnfinishedJobs = 1;

		return Job;
	}

	//-----------------------------------------------------------------------------
	// Adds the job to the queue of the current worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::Run(Job* InJob)
	{
		JobQueue* Queue = GetWorkerThreadQueue();
		Queue->Push(InJob);
	}

	//-----------------------------------------------------------------------------
	// Adds the job to the queue of the current worker thread!
	//-----------------------------------------------------------------------------
	JobQueue* JobSystem::GetWorkerThreadQueue()
	{
		return &JobSystem::WorkerThreadQueues[ThreadIndex];
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::ShutdownWorkerThread(unsigned int InThreadIndex)
	{
		if (IsThreadActive.size() < InThreadIndex)
			return;

		IsThreadActive[InThreadIndex] = false;
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	Job* JobSystem::GetJobX()
	{
		JobQueue* Queue = GetWorkerThreadQueue();

		Job* job = Queue->Pop();
		if (job == nullptr)
		{
			unsigned int randomIndex = rand() % (ThreadCount - 0 + 1) + 0;

			JobQueue* StealQueue = &WorkerThreadQueues[randomIndex];
			if (StealQueue == Queue)
			{
				// don't try to steal from ourselves
				std::this_thread::yield();
				return nullptr;
			}

			Job* stolenJob = StealQueue->Steal();

			if (stolenJob == nullptr)
			{
				// we couldn't steal a job from the other queue either, so we just yield our time slice for now
				std::this_thread::yield();
				return nullptr;
			}
			return stolenJob;
		}

		return job;
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	bool JobSystem::IsEmpty(Job* InJob)
	{
		return (InJob->UnfinishedJobs == 0);
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::Execute(Job* InJob)
	{
		(InJob->Function)(InJob);
		Finish(InJob);
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::Finish(Job* InJob)
	{
		const unsigned int UnfinishedJobs = (--InJob->UnfinishedJobs);

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
	Job* JobSystem::Allocate()
	{
		const unsigned int index = AllocatedJobs++;
		return &JobAllocator[index & (MAX_JOB_COUNT - 1u)];
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	bool JobSystem::HasJobCompleted(const Job* InJob)
	{
		return (InJob->UnfinishedJobs == 0);
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::AddContinuation(Job* InAncestor, Job* InContinuation)
	{
		const int32_t Count = InAncestor->ContinuationCount.fetch_add(1);
		InAncestor->Continuations[Count - 1] = InContinuation;
	}

	//-----------------------------------------------------------------------------
	// Closes a given worker thread!
	//-----------------------------------------------------------------------------
	void JobSystem::Wait(const Job* InJob)
	{
		// wait until the job has completed. in the meantime, work on any other job.
		while (!HasJobCompleted(InJob))
		{
			Job* NextJob = GetJobX();
			if (NextJob != nullptr)
			{
				Execute(NextJob);
			}
		}
	}
}