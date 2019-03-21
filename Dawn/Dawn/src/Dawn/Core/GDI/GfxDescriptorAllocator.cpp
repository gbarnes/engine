#include "GfxDescriptorAllocator.h"

namespace Dawn
{
	GfxDescriptorAllocator::GfxDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptorPerHeap)
		: HeapType(InType)
		, NumDescriptorsPerHeap(InNumDescriptorPerHeap)
	{
	}

	GfxDescriptorAllocator::~GfxDescriptorAllocator()
	{
	}


	std::shared_ptr<GfxDescriptorAllocatorPage> GfxDescriptorAllocator::CreateAllocatorPage()
	{
		auto newPage = std::make_shared<GfxDescriptorAllocatorPage>(HeapType, NumDescriptorsPerHeap);
		HeapPool.emplace_back(newPage);
		AvailableHeaps.insert(HeapPool.size() - 1);
		return newPage;
	}

	GfxDescriptorAllocation GfxDescriptorAllocator::Allocate(u32 InNumDescriptors)
	{
		std::lock_guard<std::mutex> lock(AllocationMutex);

		Dawn::GfxDescriptorAllocation allocation;

		for (auto it = AvailableHeaps.begin(); it != AvailableHeaps.end();)
		{
			auto allocatorPage = HeapPool[*it];
			allocation = allocatorPage->Allocate(InNumDescriptors);
			if (allocatorPage->NumFreeHandles() == 0)
			{
				it = AvailableHeaps.erase(it);
			}

			if (!allocation.IsNull())
			{
				break;
			}
		}

		if (allocation.IsNull())
		{
			NumDescriptorsPerHeap = std::max(NumDescriptorsPerHeap, InNumDescriptors);
			auto newPage = CreateAllocatorPage();
			allocation = newPage->Allocate(InNumDescriptors);
		}

		return allocation;
	}

	void GfxDescriptorAllocator::ReleaseStaleDescriptors(uint64_t InFrameNumber)
	{
		std::lock_guard<std::mutex> lock(AllocationMutex);
		for (size_t i = 0; i < HeapPool.size(); ++i)
		{
			auto page = HeapPool[i];
			page->ReleaseStaleDescriptors(InFrameNumber);
			if (page->NumFreeHandles() > 0)
			{
				AvailableHeaps.insert(i);
			}
		}
	}
}