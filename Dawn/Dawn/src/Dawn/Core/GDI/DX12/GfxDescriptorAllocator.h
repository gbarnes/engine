#pragma once
#ifdef USE_DX12_GFX

#include "inc_gfx_types.h"
#include <set>
#include <mutex>

#include "GfxDescriptorAllocatorPage.h"
#include "GfxDescriptorAllocation.h"

namespace Dawn
{
	typedef std::vector<std::shared_ptr< GfxDescriptorAllocatorPage>> DescriptorHeapPool;

	class GfxDescriptorAllocator
	{
	public:
		GfxDescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptorsPerHeap);
		virtual ~GfxDescriptorAllocator();

		GfxDescriptorAllocation Allocate(u32 InNumDescriptors = 1);

		void ReleaseStaleDescriptors(uint64_t InFrameNumber);
	private:
		

		std::shared_ptr<GfxDescriptorAllocatorPage> CreateAllocatorPage();

		D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
		u32 NumDescriptorsPerHeap;
		DescriptorHeapPool HeapPool;
		std::set<size_t> AvailableHeaps;
		std::mutex AllocationMutex;
	};

}
#endif