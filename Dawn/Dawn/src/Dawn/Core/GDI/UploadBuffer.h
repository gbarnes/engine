#pragma once

#include "inc_gfx_types.h"
#include <memory>
#include <deque>



//
// This class is based on the sample from 3dgep.com
//

namespace Dawn
{
	class UploadBuffer
	{
	public:
		struct Allocation
		{
			void* Cpu;
			D3D12_GPU_VIRTUAL_ADDRESS Gpu;
		};
		
		explicit UploadBuffer(u64 InPageSize = _2MB);

		u64 GetPageSize() const { return PageSize; }
		
		Allocation Allocate(u64 InSizeInBytes, u64 InAlignment);
		void Reset();

	private:
		struct Page
		{
			Page(u64 InSizeInBytes);
			~Page();
			bool HasSpace(u64 InSizeInBytes, u64 InAlignment);
			Allocation Allocate(u64 InSizeInBytes, u64 InAlignment);
			void Reset();

		private:
			u64 PageSize;
			u64 Offset;
			ComPtr<CGfxResource> ResourceInstance;

			void* CpuPtr;
			D3D12_GPU_VIRTUAL_ADDRESS GpuPtr;
		};

		

		std::shared_ptr<Page> RequestPage();

		u64 PageSize;
		std::shared_ptr<Page> CurrentPage;
		std::deque<std::shared_ptr<UploadBuffer::Page>> PagePool;
		std::deque<std::shared_ptr<UploadBuffer::Page>> AvailablePages;
	};
}