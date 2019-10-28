
//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "JobSystem.h"
#include <mutex>
#include <thread>

namespace Dawn
{
	std::atomic_bool ThreadsRun = false;
	unsigned int JobSystem::ThreadCount = 1;
	thread_local unsigned int JobSystem::ThreadIndex = 0;

	std::vector<std::thread> JobSystem::WorkerThreads;
	JobQueue JobSystem::WorkerThreadQueues[];

	thread_local Job JobSystem::JobAllocator[NUMBER_OF_JOBS];
	thread_local unsigned int JobSystem::AllocatedJobs = 0u;

	void JobSystem::Initialize()
	{
		ThreadCount = 1;//std::thread::hardware_concurrency();
		SpawnWorkerThreads();
	}

	void JobSystem::Shutdown()
	{
		ThreadsRun = false;
		for (auto& thread : WorkerThreads)
			thread.detach();
	}

	void JobSystem::TickWorkerThread(int InThreadIndex)
	{
		JobSystem::ThreadIndex = InThreadIndex;

		while (ThreadsRun)
		{
			Job* Job = ReturnJob();
			if (Job != nullptr)
			{
				Execute(Job);
			}
		}
	}

	EResult JobSystem::SpawnWorkerThreads()
	{
		ThreadsRun = true;

		for (unsigned int i = 0; i < ThreadCount; ++i)
		{
			std::thread t(TickWorkerThread, i);
			WorkerThreads.push_back(std::move(t));
		}

		return EResult_OK;
	}


	Job* JobSystem::CreateJob(JobFunction InFunction)
	{
		// the allocation should maybe done differently? Pool Allocator or something
		Job* Job = Allocate();
		Job->Function = InFunction;
		Job->Parent = nullptr;
		Job->UnfinishedJobs = 1;

		return Job;
	}

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

	void JobSystem::Run(Job* InJob)
	{
		JobQueue* Queue = GetWorkerThreadQueue();
		Queue->Push(InJob);
	}


	JobQueue* JobSystem::GetWorkerThreadQueue()
	{
		return &JobSystem::WorkerThreadQueues[ThreadIndex];
	}


	Job* JobSystem::ReturnJob()
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
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));
				return nullptr;
			}

			Job* stolenJob = StealQueue->Steal();

			if (stolenJob == nullptr)
			{
				// we couldn't steal a job from the other queue either, so we just yield our time slice for now
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));
				return nullptr;
			}
			return stolenJob;
		}

		return job;
	}

	bool JobSystem::IsEmpty(Job* InJob)
	{
		return (InJob->UnfinishedJobs == 0);
	}


	void JobSystem::Execute(Job* InJob)
	{
		(InJob->Function)(InJob);
		Finish(InJob);
	}


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


	Job* JobSystem::Allocate()
	{
		const unsigned int index = AllocatedJobs++;
		return &JobAllocator[index & (MAX_JOB_COUNT - 1u)];
	}


	bool JobSystem::HasJobCompleted(const Job* InJob)
	{
		return (InJob->UnfinishedJobs == 0);
	}


	void JobSystem::AddContinuation(Job* InAncestor, Job* InContinuation)
	{
		const int32_t Count = InAncestor->ContinuationCount.fetch_add(1);
		InAncestor->Continuations[Count - 1] = InContinuation;
	}

	void JobSystem::Wait(const Job* InJob)
	{
		// wait until the job has completed. in the meantime, work on any other job.
		while (!HasJobCompleted(InJob))
		{
			Job* NextJob = ReturnJob();
			if (NextJob != nullptr)
			{
				Execute(NextJob);
			}
		}
	}
}