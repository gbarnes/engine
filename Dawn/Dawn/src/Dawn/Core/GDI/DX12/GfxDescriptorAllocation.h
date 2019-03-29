#pragma once
#ifdef USE_DX12_GFX
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxDescriptorAllocatorPage;

	class GfxDescriptorAllocation
	{
	public:
		GfxDescriptorAllocation();
		GfxDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InHandle, u32 InNumHandles, u32 InDescriptorSize,
			std::shared_ptr<GfxDescriptorAllocatorPage> Inpage);

		~GfxDescriptorAllocation();

		GfxDescriptorAllocation(const GfxDescriptorAllocation&) = delete;
		GfxDescriptorAllocation& operator=(const GfxDescriptorAllocation&) = delete;

		// Move is allowed.
		GfxDescriptorAllocation(GfxDescriptorAllocation&& allocation);
		GfxDescriptorAllocation& operator=(GfxDescriptorAllocation&& other);

		bool IsNull() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(u32 InOffset = 0) const;
		u32 GetNumHandles() const;
		std::shared_ptr<GfxDescriptorAllocatorPage> GetDescriptorAllocatorPage() const;
	private:
		// Free the descriptor back to the heap it came from.
		void Free();

		// The base descriptor.
		D3D12_CPU_DESCRIPTOR_HANDLE Descriptor;
		// The number of descriptors in this allocation.
		u32 NumHandles;
		// The offset to the next descriptor.
		u32 DescriptorSize;

		// A pointer back to the original page where this allocation came from.
		std::shared_ptr<GfxDescriptorAllocatorPage> Page;
	};
}
#endif