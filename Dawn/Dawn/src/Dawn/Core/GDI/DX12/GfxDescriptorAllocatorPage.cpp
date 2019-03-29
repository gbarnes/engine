#ifdef USE_DX12_GFX
#include "GfxDescriptorAllocatorPage.h"
#include "GfxBackend.h"
#include "GfxDescriptorAllocator.h"
#include "GfxDescriptorAllocation.h"

namespace Dawn
{
	GfxDescriptorAllocatorPage::GfxDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors)
		: HeapType(InType)
		, NumDescriptorsInHeap(InNumDescriptors)
	{
		// TODO: mhm this isn't very nice since it could fail?!
		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.Type = InType;
		Desc.NumDescriptors = InNumDescriptors;

		ThrowIfFailed(GfxBackend::GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&this->InternalHeapDescriptor)));

		BaseDescriptor = InternalHeapDescriptor->GetCPUDescriptorHandleForHeapStart();
		DescriptorHandleIncrementSize = GfxBackend::GetDevice()->GetDescriptorHandleIncrementSize(InType);
		VarNumFreeHandles = InNumDescriptors;

		AddNewBlock(0, VarNumFreeHandles);
	}

	D3D12_DESCRIPTOR_HEAP_TYPE GfxDescriptorAllocatorPage::GetHeapType() const
	{
		return HeapType;
	}

	u32 GfxDescriptorAllocatorPage::NumFreeHandles() const
	{
		return VarNumFreeHandles;
	}

	bool GfxDescriptorAllocatorPage::HasSpace(u32 InNumDescriptors) const
	{
		return FreeListBySize.lower_bound(InNumDescriptors) != FreeListBySize.end();
	}

	void GfxDescriptorAllocatorPage::AddNewBlock(u32 InOffset, u32 InNumDescriptors)
	{
		auto offsetIt = FreeListByOffset.emplace(InOffset, InNumDescriptors);
		auto sizeIt = FreeListBySize.emplace(InNumDescriptors, offsetIt.first);
		offsetIt.first->second.FreeListBySizeIt = sizeIt;
	}

	GfxDescriptorAllocation GfxDescriptorAllocatorPage::Allocate(u32 InNumDescriptors)
	{
		std::lock_guard<std::mutex> lock(AllocationMutex);
		if (InNumDescriptors > VarNumFreeHandles)
		{
			return GfxDescriptorAllocation();
		}

		auto smallestBlockIt = FreeListBySize.lower_bound(InNumDescriptors);
		if (smallestBlockIt == FreeListBySize.end())
			return GfxDescriptorAllocation();

		auto blockSize = smallestBlockIt->first;
		auto offsetIt = smallestBlockIt->second;
		auto offset = offsetIt->first;

		FreeListBySize.erase(smallestBlockIt);
		FreeListByOffset.erase(offsetIt);

		auto newOffset = offset + InNumDescriptors;
		auto newSize = blockSize - InNumDescriptors;

		if (newSize > 0)
			AddNewBlock(newOffset, newSize);

		VarNumFreeHandles -= InNumDescriptors;
		return GfxDescriptorAllocation(CD3DX12_CPU_DESCRIPTOR_HANDLE(BaseDescriptor, offset, DescriptorHandleIncrementSize), InNumDescriptors,
			DescriptorHandleIncrementSize, shared_from_this());
	}

	u32 GfxDescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		return static_cast<uint32_t>(handle.ptr - BaseDescriptor.ptr) / DescriptorHandleIncrementSize;
	}

	void GfxDescriptorAllocatorPage::Free(GfxDescriptorAllocation&& InDescriptor, uint64_t InFrameNumber)
	{
		// Compute the offset of the descriptor within the descriptor heap.
		auto offset = ComputeOffset(InDescriptor.GetDescriptorHandle());

		std::lock_guard<std::mutex> lock(AllocationMutex);

		// Don't add the block directly to the free list until the frame has completed.
		StaleDescriptors.emplace(offset, InDescriptor.GetNumHandles(), InFrameNumber);
	}

	void GfxDescriptorAllocatorPage::FreeBlock(u32 InOffset, u32 InNumDescriptors)
	{
		// Find the first element whose offset is greater than the specified offset.
		// This is the block that should appear after the block that is being freed.
		auto nextBlockIt = FreeListByOffset.upper_bound(InOffset);

		// Find the block that appears before the block being freed.
		auto prevBlockIt = nextBlockIt;
		// If it's not the first block in the list.
		if (prevBlockIt != FreeListByOffset.begin())
		{
			// Go to the previous block in the list.
			--prevBlockIt;
		}
		else
		{
			// Otherwise, just set it to the end of the list to indicate that no
			// block comes before the one being freed.
			prevBlockIt = FreeListByOffset.end();
		}

		// Add the number of free handles back to the heap.
		// This needs to be done before merging any blocks since merging
		// blocks modifies the numDescriptors variable.
		VarNumFreeHandles += InNumDescriptors;

		if (prevBlockIt != FreeListByOffset.end() &&
			InOffset == prevBlockIt->first + prevBlockIt->second.Size)
		{
			// The previous block is exactly behind the block that is to be freed.
			//
			// PrevBlock.Offset           Offset
			// |                          |
			// |<-----PrevBlock.Size----->|<------Size-------->|
			//

			// Increase the block size by the size of merging with the previous block.
			InOffset = prevBlockIt->first;
			InNumDescriptors += prevBlockIt->second.Size;

			// Remove the previous block from the free list.
			FreeListBySize.erase(prevBlockIt->second.FreeListBySizeIt);
			FreeListByOffset.erase(prevBlockIt);
		}

		if (nextBlockIt != FreeListByOffset.end() &&
			InOffset + InNumDescriptors == nextBlockIt->first)
		{
			// The next block is exactly in front of the block that is to be freed.
			//
			// Offset               NextBlock.Offset 
			// |                    |
			// |<------Size-------->|<-----NextBlock.Size----->|

			// Increase the block size by the size of merging with the next block.
			InNumDescriptors += nextBlockIt->second.Size;

			// Remove the next block from the free list.
			FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIt);
			FreeListByOffset.erase(nextBlockIt);
		}

		// Add the freed block to the free list.
		AddNewBlock(InOffset, InNumDescriptors);
	}

	void GfxDescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t InFrameNumber)
	{
		std::lock_guard<std::mutex> lock(AllocationMutex);

		while (!StaleDescriptors.empty() && 
			StaleDescriptors.front().FrameNumber <= InFrameNumber)
		{
			auto& staleDescriptor = StaleDescriptors.front();

			// The offset of the descriptor in the heap.
			auto offset = staleDescriptor.Offset;
			// The number of descriptors that were allocated.
			auto numDescriptors = staleDescriptor.Size;

			FreeBlock(offset, numDescriptors);

			StaleDescriptors.pop();
		}
	}
}
#endif