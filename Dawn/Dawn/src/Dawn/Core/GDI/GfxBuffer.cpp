#include "GfxBuffer.h"

namespace Dawn
{
	GfxBuffer::GfxBuffer(const std::wstring& name)
		: GfxResource(name)
	{}

	GfxBuffer::GfxBuffer(const D3D12_RESOURCE_DESC& resDesc,
		size_t numElements, size_t elementSize,
		const std::wstring& name)
		: GfxResource(resDesc, nullptr, name)
	{
		CreateViews(numElements, elementSize);
	}

	void GfxBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		throw std::exception("Unimplemented function.");
	}
}