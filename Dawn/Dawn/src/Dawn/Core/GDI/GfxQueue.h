#pragma once
#include "inc_core.h"
#include <queue>
#include "inc_gfx_types.h"
#include <d3d12.h>
#include <atomic>               // For std::atomic_bool
#include <cstdint>              // For uint64_t
#include <condition_variable>   // For std::condition_variable.
#include "GfxThreadSafeQueue.h"

namespace Dawn
{
	class GfxCmdList;
	using CommandListEntry = std::tuple<uint64_t, std::shared_ptr<GfxCmdList> >;

	class GfxQueue
	{
	public:
		GfxQueue(D3D12_COMMAND_LIST_TYPE InType);
		virtual ~GfxQueue();


		// Get an available command list from the command queue.
		std::shared_ptr<GfxCmdList> GetCommandList();

		// Execute a command list.
		// Returns the fence value to wait for for this command list.
		uint64_t ExecuteCommandList(std::shared_ptr<GfxCmdList> commandList);
		uint64_t ExecuteCommandLists(const std::vector<std::shared_ptr<GfxCmdList> >& commandLists);


		uint64_t Signal();
		bool IsFenceComplete(uint64_t InFenceValue);
		void WaitForFenceValue(uint64_t InFenceValue);
		void Flush();
		void Reset();

		// Wait for another command queue to finish.
		void Wait(const GfxQueue& other);


		ComPtr<ID3D12CommandQueue> GetD3D12CommandQueue() const;
	private:
		
		// Free any command lists that are finished processing on the command queue.
		void ProccessInFlightCommandLists();


		D3D12_COMMAND_LIST_TYPE CommandListType;
		ComPtr<ID3D12CommandQueue> CmdQueue;
		ComPtr<ID3D12Fence> Fence;
		HANDLE FenceEvent;
		std::atomic_uint64_t FenceValue;


		ThreadSafeQueue<CommandListEntry>               InFlightCommandLists;
		ThreadSafeQueue<std::shared_ptr<GfxCmdList> >  AvailableCommandLists;

		// A thread to process in-flight command lists.
		std::thread ProcessInFlightCommandListsThread;
		std::atomic_bool bProcessInFlightCommandLists;
		std::mutex ProcessInFlightCommandListsThreadMutex;
		std::condition_variable ProcessInFlightCommandListsThreadCV;
	};
}

