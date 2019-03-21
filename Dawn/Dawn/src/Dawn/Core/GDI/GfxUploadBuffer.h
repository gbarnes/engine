#pragma once

#include "inc_gfx_types.h"
#include <memory>
#include <deque>



//
// This class is based on the sample from 3dgep.com
//

namespace Dawn
{
	class GfxUploadBuffer
	{
	public:
		struct Allocation
		{
			void* Cpu;
			D3D12_GPU_VIRTUAL_ADDRESS Gpu;
		};
		
		explicit GfxUploadBuffer(u128 InPageSize = _2MB);

		u128 GetPageSize() const { return PageSize; }
		
		Allocation Allocate(u128 InSizeInBytes, u128 InAlignment);
		void Reset();

	private:
		struct GfxPage
		{
			GfxPage(u128 InSizeInBytes);
			~GfxPage();
			bool HasSpace(u128 InSizeInBytes, u128 InAlignment);
			Allocation Allocate(u128 InSizeInBytes, u128 InAlignment);
			void Reset();

		private: 
			u128 PageSize;
			u128 Offset;
			ComPtr<ID3D12Resource> ResourceInstance;

			void* CpuPtr;
			D3D12_GPU_VIRTUAL_ADDRESS GpuPtr;
		};

		

		std::shared_ptr<GfxPage> RequestPage();

		u128 PageSize;
		std::shared_ptr<GfxPage> CurrentPage;
		std::deque<std::shared_ptr<GfxUploadBuffer::GfxPage>> PagePool;
		std::deque<std::shared_ptr<GfxUploadBuffer::GfxPage>> AvailablePages;
	};
}