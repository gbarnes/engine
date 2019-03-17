#include "GfxQueue.h"
#include "GfxDevice.h"
#include "GfxBackend.h"

namespace Dawn
{
	//
	// 
	//
	CGfxQueue::CGfxQueue(D3D12_COMMAND_LIST_TYPE InType)
		: FenceValue(0), CommandListType(InType)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = CommandListType;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		CGfxDevice* Device = GfxBackend::GetDevice();
		assert(Device && "GfxDevice is null!");

		ThrowIfFailed(Device->GetD3D12Device()->CreateCommandQueue(&desc, IID_PPV_ARGS(&CmdQueue)));
		Fence = Device->CreateFence();

		FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(FenceEvent && "Failed to create fence event handle.");

	}

	CGfxQueue::~CGfxQueue()
	{
		::CloseHandle(FenceEvent);
	}

	// Get an available command list from the command queue.
	ComPtr<CGfxCmdList> CGfxQueue::GetCommandList()
	{
		ComPtr<CGfxCmdAllocator> CmdAllocator;
		ComPtr<CGfxCmdList> CmdList;

		if (!CommandAllocatorQueue.empty() && IsFenceComplete(CommandAllocatorQueue.front().FenceValue))
		{
			CmdAllocator = CommandAllocatorQueue.front().CommandAllocator;
			CommandAllocatorQueue.pop();
			ThrowIfFailed(CmdAllocator->Reset());
		}
		else
		{
			CmdAllocator = CreateCommandAllocator();
		}

		if (!CommandListQueue.empty())
		{
			CmdList = CommandListQueue.front();
			CommandListQueue.pop();
			ThrowIfFailed(CmdList->Reset(CmdAllocator.Get(), nullptr));
		}
		else
		{
			CmdList = CreateCommandList(CmdAllocator);
			ThrowIfFailed(CmdList->Reset(CmdAllocator.Get(), nullptr));
	
		}

		// Associate the command allocator with the command list so that it can be
		// retrieved when the command list is executed.
		ThrowIfFailed(CmdList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), CmdAllocator.Get()));

		return CmdList;

	}

	// Execute a command list.
	// Returns the fence value to wait for for this command list.
	u64 CGfxQueue::ExecuteCommandList(ComPtr<CGfxCmdList> InCommandList)
	{
		InCommandList->Close();

		ID3D12CommandAllocator* CommandAllocator;
		UINT dataSize = sizeof(CommandAllocator);
		ThrowIfFailed(InCommandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &CommandAllocator));

		ID3D12CommandList* const ppCommandLists[] = {
			InCommandList.Get()
		};

		CmdQueue->ExecuteCommandLists(1, ppCommandLists);
		u64 FenceValue = Signal();

		CommandAllocatorQueue.emplace(SCommandAllocatorEntry{ FenceValue, CommandAllocator });
		CommandListQueue.push(InCommandList);
		CommandAllocator->Release();

		return FenceValue;
	}

	u64 CGfxQueue::Signal()
	{
		u64 FenceValueForSignal = ++FenceValue;
		ThrowIfFailed(CmdQueue->Signal(Fence.Get(), FenceValueForSignal));

		return FenceValueForSignal;
	}

	bool CGfxQueue::IsFenceComplete(u64 InFenceValue)
	{
		return Fence->GetCompletedValue() >= InFenceValue;
	}

	void CGfxQueue::WaitForFenceValue(u64 InFenceValue)
	{
		if (!IsFenceComplete(InFenceValue))
		{
			ThrowIfFailed(Fence->SetEventOnCompletion(InFenceValue, FenceEvent));
			::WaitForSingleObject(FenceEvent, static_cast<DWORD>(std::chrono::milliseconds::max().count()));
		}
	}

	void CGfxQueue::Flush()
	{
		u64 FenceValueForSignal = Signal();
		WaitForFenceValue(FenceValueForSignal);
	}

	void CGfxQueue::Reset()
	{
		CmdQueue.Reset();
		Fence.Reset();
	}

	ComPtr<CGfxInternalQueue> CGfxQueue::GetD3D12CommandQueue() const
	{
		return this->CmdQueue;
	}

	ComPtr<CGfxCmdAllocator> CGfxQueue::CreateCommandAllocator()
	{
		CGfxDevice* Device = GfxBackend::GetDevice();
		assert(Device && "GfxDevice is null!");
		return Device->CreateCmdAllocator(CommandListType);
	}

	ComPtr<CGfxCmdList> CGfxQueue::CreateCommandList(ComPtr<CGfxCmdAllocator> InAllocator)
	{
		CGfxDevice* Device = GfxBackend::GetDevice();
		assert(Device && "GfxDevice is null!");
		return Device->CreateCmdList(InAllocator, CommandListType);
	}
}
