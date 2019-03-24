#include "GfxConstantBuffer.h"
#include "GfxBackend.h"
#include "Core/Math.h"

namespace Dawn
{
	GfxConstantBuffer::GfxConstantBuffer(const std::wstring & name)
		: GfxBuffer(name)
		, m_SizeInBytes(0)
	{
		m_ConstantBufferView = GfxBackend::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	GfxConstantBuffer::~GfxConstantBuffer()
	{}

	void GfxConstantBuffer::CreateViews(size_t numElements, size_t elementSize)
	{
		m_SizeInBytes = numElements * elementSize;

		D3D12_CONSTANT_BUFFER_VIEW_DESC d3d12ConstantBufferViewDesc;
		d3d12ConstantBufferViewDesc.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
		d3d12ConstantBufferViewDesc.SizeInBytes = static_cast<UINT>(Math::AlignUp(m_SizeInBytes, 16));

		auto device = GfxBackend::GetDevice();

		device->CreateConstantBufferView(&d3d12ConstantBufferViewDesc, m_ConstantBufferView.GetDescriptorHandle());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxConstantBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const
	{
		throw std::exception("ConstantBuffer::GetShaderResourceView should not be called.");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxConstantBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const
	{
		throw std::exception("ConstantBuffer::GetUnorderedAccessView should not be called.");
	}
}