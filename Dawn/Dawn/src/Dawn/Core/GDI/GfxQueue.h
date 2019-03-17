#pragma once
#include "inc_core.h"
#include <queue>
#include "inc_gfx_types.h"
#include <d3d12.h>

namespace Dawn
{
	// Keep track of command allocators that are "in-flight"
	struct SCommandAllocatorEntry
	{
		u64 FenceValue;
		ComPtr<CGfxCmdAllocator> CommandAllocator;
	};

	using CommandAllocatorQueue = std::queue<SCommandAllocatorEntry>;
	using CommandListQueue = std::queue< ComPtr<CGfxCmdList> >;

	class CGfxQueue
	{
	public:
		CGfxQueue(D3D12_COMMAND_LIST_TYPE InType);
		virtual ~CGfxQueue();

		// Get an available command list from the command queue.
		ComPtr<CGfxCmdList> GetCommandList();

		// Execute a command list.
		// Returns the fence value to wait for for this command list.
		u64 ExecuteCommandList(ComPtr<CGfxCmdList> InCommandList);

		u64 Signal();
		bool IsFenceComplete(u64 InFenceValue);
		void WaitForFenceValue(u64 InFenceValue);
		void Flush();
		void Reset();

		ComPtr<CGfxInternalQueue> GetD3D12CommandQueue() const;

	protected:
		ComPtr<CGfxCmdAllocator> CreateCommandAllocator();
		ComPtr<CGfxCmdList> CreateCommandList(ComPtr<CGfxCmdAllocator> InAllocator);

	private:
		
		D3D12_COMMAND_LIST_TYPE CommandListType;
		ComPtr<CGfxInternalQueue> CmdQueue;
		ComPtr<CGfxFence> Fence;
		HANDLE FenceEvent;
		u64 FenceValue;

		CommandAllocatorQueue CommandAllocatorQueue;
		CommandListQueue CommandListQueue;
	};
}

