#include "stdafx.h"
#ifdef USE_DX12_GFX
#include "GfxQueue.h"
#include "GfxBackend.h"

#include "GfxCmdList.h"
#include "GfxResourceStateTracker.h"

namespace Dawn
{
	//
	// 
	//
	GfxQueue::GfxQueue(D3D12_COMMAND_LIST_TYPE InType)
		: FenceValue(0), CommandListType(InType), bProcessInFlightCommandLists(true)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = CommandListType;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		auto device = GfxBackend::GetDevice();
		assert(device && "GfxDevice is null!");

		ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&CmdQueue)));
		ThrowIfFailed(device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

		switch (InType)
		{
		case D3D12_COMMAND_LIST_TYPE_COPY:
			CmdQueue->SetName(L"Copy Command Queue");
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			CmdQueue->SetName(L"Compute Command Queue");
			break;
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			CmdQueue->SetName(L"Direct Command Queue");
			break;
		}

		ProcessInFlightCommandListsThread = std::thread(&GfxQueue::ProccessInFlightCommandLists, this);
	}

	GfxQueue::~GfxQueue()
	{
		bProcessInFlightCommandLists = false;
		ProcessInFlightCommandListsThread.join();
	}

	// Get an available command list from the command queue.
	std::shared_ptr<GfxCmdList> GfxQueue::GetCommandList()
	{
		std::shared_ptr<GfxCmdList> commandList;

		// If there is a command list on the queue.
		if (!AvailableCommandLists.Empty())
		{
			AvailableCommandLists.TryPop(commandList);
		}
		else
		{
			// Otherwise create a new command list.
			commandList = std::make_shared<GfxCmdList>(CommandListType);
		}

		return commandList;
	}

	// Execute a command list.
	// Returns the fence value to wait for for this command list.
	uint64_t GfxQueue::ExecuteCommandList(std::shared_ptr<GfxCmdList> commandList)
	{
		return ExecuteCommandLists(std::vector<std::shared_ptr<GfxCmdList> >({ commandList }));
	}

	uint64_t GfxQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<GfxCmdList> >& commandLists)
	{
		GfxResourceStateTracker::Lock();

		// Command lists that need to put back on the command list queue.
		std::vector<std::shared_ptr<GfxCmdList> > toBeQueued;
		toBeQueued.reserve(commandLists.size() * 2);        // 2x since each command list will have a pending command list.

		// Generate mips command lists.
		std::vector<std::shared_ptr<GfxCmdList> > generateMipsCommandLists;
		generateMipsCommandLists.reserve(commandLists.size());

		// Command lists that need to be executed.
		std::vector<ID3D12CommandList*> d3d12CommandLists;
		d3d12CommandLists.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

		for (auto commandList : commandLists)
		{
			auto pendingCommandList = GetCommandList();
			bool hasPendingBarriers = commandList->Close(*pendingCommandList);
			pendingCommandList->Close();
			// If there are no pending barriers on the pending command list, there is no reason to 
			// execute an empty command list on the command queue.
			if (hasPendingBarriers)
			{
				d3d12CommandLists.push_back(pendingCommandList->GetGraphicsCommandList().Get());
			}
			d3d12CommandLists.push_back(commandList->GetGraphicsCommandList().Get());

			toBeQueued.push_back(pendingCommandList);
			toBeQueued.push_back(commandList);

			auto generateMipsCommandList = commandList->GetGenerateMipsCommandList();
			if (generateMipsCommandList)
			{
				generateMipsCommandLists.push_back(generateMipsCommandList);
			}
		}

		UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());
		CmdQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
		uint64_t fenceValue = Signal();

		GfxResourceStateTracker::Unlock();

		// Queue command lists for reuse.
		for (auto commandList : toBeQueued)
		{
			InFlightCommandLists.Push({ fenceValue, commandList });
		}

		// If there are any command lists that generate mips then execute those
		// after the initial resource command lists have finished.
		if (generateMipsCommandLists.size() > 0)
		{
			auto computeQueue = GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
			computeQueue->Wait(*this);
			computeQueue->ExecuteCommandLists(generateMipsCommandLists);
		}

		return fenceValue;
	}

	void GfxQueue::Wait(const GfxQueue& other)
	{
		CmdQueue->Wait(other.Fence.Get(), other.FenceValue);
	}

	uint64_t GfxQueue::Signal()
	{
		u128 FenceValueForSignal = ++FenceValue;
		ThrowIfFailed(CmdQueue->Signal(Fence.Get(), FenceValueForSignal));

		return FenceValueForSignal;
	}

	bool GfxQueue::IsFenceComplete(uint64_t InFenceValue)
	{
		return Fence->GetCompletedValue() >= InFenceValue;
	}

	void GfxQueue::WaitForFenceValue(uint64_t InFenceValue)
	{
		if (!IsFenceComplete(InFenceValue))
		{
			auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(event && "Failed to create fence event handle.");

			// Is this function thread safe?
			Fence->SetEventOnCompletion(InFenceValue, event);
			::WaitForSingleObject(event, DWORD_MAX);

			::CloseHandle(event);
		}
	}

	void GfxQueue::Flush()
	{
		std::unique_lock<std::mutex> lock(ProcessInFlightCommandListsThreadMutex);
		ProcessInFlightCommandListsThreadCV.wait(lock, [this] { return InFlightCommandLists.Empty(); });


		WaitForFenceValue(FenceValue);
	}

	void GfxQueue::Reset()
	{
		CmdQueue.Reset();
		Fence.Reset();
	}

	ComPtr<ID3D12CommandQueue> GfxQueue::GetD3D12CommandQueue() const
	{
		return this->CmdQueue;
	}

	void GfxQueue::ProccessInFlightCommandLists()
	{
		std::unique_lock<std::mutex> lock(ProcessInFlightCommandListsThreadMutex, std::defer_lock);

		while (bProcessInFlightCommandLists)
		{
			CommandListEntry commandListEntry;

			lock.lock();
			while (InFlightCommandLists.TryPop(commandListEntry))
			{
				auto fenceValue = std::get<0>(commandListEntry);
				auto commandList = std::get<1>(commandListEntry);

				WaitForFenceValue(fenceValue);

				commandList->Reset();

				AvailableCommandLists.Push(commandList);
			}
			lock.unlock();
			ProcessInFlightCommandListsThreadCV.notify_one();

			std::this_thread::yield();
		}
	}
}
#endif