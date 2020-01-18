#include "stdafx.h"
#ifdef USE_DX12_GFX
#include "GfxDescriptorAllocation.h"
#include "GfxDescriptorAllocatorPage.h"
#include "Application.h"

namespace Dawn
{

	GfxDescriptorAllocation::GfxDescriptorAllocation()
		: Descriptor{ 0 }
		, NumHandles(0)
		, DescriptorSize(0)
		, Page(nullptr)
	{}

	GfxDescriptorAllocation::GfxDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE InDescriptor
		, uint32_t InNumHandles
		, uint32_t InDescriptorSize
		, std::shared_ptr<GfxDescriptorAllocatorPage> InPage)
		: Descriptor(InDescriptor)
		, NumHandles(InNumHandles)
		, DescriptorSize(InDescriptorSize)
		, Page(InPage)
	{}

	GfxDescriptorAllocation::~GfxDescriptorAllocation()
	{
		Free();
	}

	GfxDescriptorAllocation::GfxDescriptorAllocation(GfxDescriptorAllocation&& allocation)
		: Descriptor(allocation.Descriptor)
		, NumHandles(allocation.NumHandles)
		, DescriptorSize(allocation.DescriptorSize)
		, Page(std::move(allocation.Page))
	{
		allocation.Descriptor.ptr = 0;
		allocation.NumHandles = 0;
		allocation.DescriptorSize = 0;
	}

	GfxDescriptorAllocation& GfxDescriptorAllocation::operator=(GfxDescriptorAllocation&& other)
	{
		// Free this descriptor if it points to anything.
		Free();

		Descriptor = other.Descriptor;
		NumHandles = other.NumHandles;
		DescriptorSize = other.DescriptorSize;
		Page = std::move(other.Page);

		other.Descriptor.ptr = 0;
		other.NumHandles = 0;
		other.DescriptorSize = 0;

		return *this;
	}

	void GfxDescriptorAllocation::Free()
	{
		if (!IsNull() && Page)
		{
			// TODO (gb): normally there shouldn't exist this dependency on the application layer from the gdi layer...
			Page->Free(std::move(*this), Application::GetFrameCount());

			Descriptor.ptr = 0;
			NumHandles = 0;
			DescriptorSize = 0;
			Page.reset();
		}
	}

	bool GfxDescriptorAllocation::IsNull() const
	{
		return Descriptor.ptr == 0;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxDescriptorAllocation::GetDescriptorHandle(u32 offset) const
	{
		assert(offset < NumHandles);
		return { Descriptor.ptr + (DescriptorSize * offset) };
	}

	u32 GfxDescriptorAllocation::GetNumHandles() const
	{
		return NumHandles;
	}

	std::shared_ptr<GfxDescriptorAllocatorPage> GfxDescriptorAllocation::GetDescriptorAllocatorPage() const
	{
		return Page;
	}
}
#endif