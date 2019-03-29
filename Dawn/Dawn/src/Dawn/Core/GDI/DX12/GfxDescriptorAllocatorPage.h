#pragma once
#ifdef USE_DX12_GFX
#include "inc_gfx_types.h"
#include <map>
#include <memory>
#include <mutex>
#include <queue>

namespace Dawn
{
	typedef u32 OffsetType;
	typedef u32 SizeType;
	struct FreeBlockInfo;


	typedef std::map<OffsetType, FreeBlockInfo> FreeListByOffset;
	typedef std::multimap<SizeType, FreeListByOffset::iterator> FreeListBySize;

	struct FreeBlockInfo
	{
		FreeBlockInfo(SizeType InSize)
			:Size(InSize)
		{}

		SizeType Size;
		FreeListBySize::iterator FreeListBySizeIt;
	};

	class GfxDescriptorAllocation;
	class GfxDescriptorAllocatorPage : public std::enable_shared_from_this<GfxDescriptorAllocatorPage>
	{
	public:
		GfxDescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors);

		D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

		bool HasSpace(u32 InNumDescriptors) const;
		u32 NumFreeHandles() const;

		GfxDescriptorAllocation Allocate(u32 InNumDescriptors);

		void Free(GfxDescriptorAllocation&& InDescriptorHandle, uint64_t InFrameNumber);
		void ReleaseStaleDescriptors(uint64_t InFrameNumber);

	protected:
		u32 ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE InHandle);
		void AddNewBlock(u32 InOffset, u32 InNumDescriptors);
		void FreeBlock(u32 InOffset, u32 InNumDescriptors);
	private:

		struct StaleDescriptorInfo
		{
			StaleDescriptorInfo ( OffsetType InOffset, SizeType InSize, uint64_t InFrame)
				: Offset(InOffset)
				, Size(InSize)
				, FrameNumber(InFrame)
			{}

			OffsetType Offset;
			SizeType Size;
			uint64_t FrameNumber;
		};


		FreeListByOffset FreeListByOffset;
		FreeListBySize FreeListBySize;
		std::queue< StaleDescriptorInfo> StaleDescriptors;

		ComPtr<ID3D12DescriptorHeap> InternalHeapDescriptor;
		D3D12_DESCRIPTOR_HEAP_TYPE HeapType;
		CD3DX12_CPU_DESCRIPTOR_HANDLE BaseDescriptor;
		u32 DescriptorHandleIncrementSize;
		u32 NumDescriptorsInHeap;
		u32 VarNumFreeHandles;

		std::mutex AllocationMutex;

	};

}
#endif
