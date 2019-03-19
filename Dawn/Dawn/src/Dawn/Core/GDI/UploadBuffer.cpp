#include "UploadBuffer.h"
#include "GfxDevice.h"
#include "GfxBackend.h"
#include "inc_core.h"
#include <new>

namespace Dawn
{
	UploadBuffer::UploadBuffer(u64 InPageSize)
		: PageSize(InPageSize)
	{

	}

	UploadBuffer::Allocation UploadBuffer::Allocate(u64 InSizeInBytes, u64 InAlignment)
	{
		// Note (gb): mhm exceptions arent the best
		if (InSizeInBytes > PageSize)
		{
			throw std::bad_alloc();
		}

		if (!CurrentPage || !CurrentPage->HasSpace(InSizeInBytes, InAlignment))
		{
			CurrentPage = RequestPage();
		}

		return CurrentPage->Allocate(InSizeInBytes, InAlignment);
	}

	std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage()
	{
		std::shared_ptr<UploadBuffer::Page> page;

		if (!AvailablePages.empty())
		{
			page = AvailablePages.front();
			AvailablePages.pop_front();
		}
		else
		{
			page = std::make_shared<UploadBuffer::Page>(PageSize);
			PagePool.push_back(page);
		}

		return page;
	}

	void UploadBuffer::Reset()
	{
		CurrentPage = nullptr;
		AvailablePages = PagePool;
		for (auto page : AvailablePages)
		{
			page->Reset();
		}
	}

	UploadBuffer::Page::Page(u64 InSizeInBytes)
		: PageSize(InSizeInBytes), 
		Offset(0), 
		CpuPtr(nullptr), 
		GpuPtr(D3D12_GPU_VIRTUAL_ADDRESS(0))

	{
		auto device = GfxBackend::GetDevice()->GetD3D12Device();
		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(PageSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&ResourceInstance)
		));

		GpuPtr = ResourceInstance->GetGPUVirtualAddress();
		ResourceInstance->Map(0, nullptr, &CpuPtr);
	}

	UploadBuffer::Page::~Page()
	{
		ResourceInstance->Unmap(0, nullptr);
		CpuPtr = nullptr;
		GpuPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	bool UploadBuffer::Page::HasSpace(u64 InSizeInBytes, u64 InAlignment)
	{
		// Note (gb): research how AlignUp really works I don't relly get it atm.
		u64 alignedSize = Math::AlignUp(InSizeInBytes, InAlignment);
		u64 alignedOffset = Math::AlignUp(Offset, InAlignment);

		return alignedOffset + alignedSize <= PageSize;
	}

	UploadBuffer::Allocation UploadBuffer::Page::Allocate(u64 InSizeInBytes, u64 InAlignment)
	{
		// Note (gb): Again this ulgy exceptions!
		if (!HasSpace(InSizeInBytes, InAlignment))
		{
			throw std::bad_alloc();
		}

		u64 alignedSize = Math::AlignUp(InSizeInBytes, InAlignment);
		Offset = Math::AlignUp(Offset, InAlignment);

		Allocation allocation;
		allocation.Cpu = static_cast<uint8_t*>(CpuPtr) + Offset;
		allocation.Gpu = GpuPtr + Offset;

		Offset += alignedSize;
		return allocation;
	}

	void UploadBuffer::Page::Reset()
	{
		Offset = 0;
	}

}