#include "GfxDynamicDescriptorHeap.h"
#include "GfxBackend.h"
#include "GfxDevice.h"
#include "GfxCmdList.h"
#include "GfxRootSignature.h"

namespace Dawn
{
	GfxDynamicDescriptorHeap::GfxDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptorsPerHeap)
		: DescriptorHeapType(InType)
		, NumDescriptorsPerHeap(InNumDescriptorsPerHeap)
		, DescriptorTableBitMask(0)
		, StaleDescriptorTableBitMask(0)
		, CurrentCPUDescriptorHandle(D3D12_DEFAULT)
		, CurrentGPUDescriptorHandle(D3D12_DEFAULT)
		, NumFreeHandles(0)
	{
		//DescriptorHandleIncrementSize = Application::Get().GetDescriptorHandleIncrementSize(InType);

		// Allocate space for staging CPU visible descriptors.
		DescriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(NumDescriptorsPerHeap);
	}

	GfxDynamicDescriptorHeap::~GfxDynamicDescriptorHeap()
	{
	}

	void GfxDynamicDescriptorHeap::ParseRootSignature(const GfxRootSignature& InRootSignature)
	{
		// If the root signature changes, all descriptors must be (re)bound to the
		// command list.
		StaleDescriptorTableBitMask = 0;

		const auto& rootSignatureDesc = InRootSignature.GetRootSignatureDesc();

		// Get a bit mask that represents the root parameter indices that match the 
		// descriptor heap type for this dynamic descriptor heap.
		DescriptorTableBitMask = InRootSignature.GetDescriptorTableBitMask(DescriptorHeapType);
		u32 descriptorTableBitMask = DescriptorTableBitMask;

		uint32_t currentOffset = 0;
		DWORD rootIndex;
		while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootSignatureDesc.NumParameters)
		{
			uint32_t numDescriptors = InRootSignature.GetNumDescriptors(rootIndex);

			DescriptorTableCache& descriptorTableCache = DescriptorTableCaches[rootIndex];
			descriptorTableCache.NumDescriptors = numDescriptors;
			descriptorTableCache.BaseDescriptor = DescriptorHandleCache.get() + currentOffset;

			currentOffset += numDescriptors;

			// Flip the descriptor table bit so it's not scanned again for the current index.
			descriptorTableBitMask ^= (1 << rootIndex);
		}

		// Make sure the maximum number of descriptors per descriptor heap has not been exceeded.
		assert(currentOffset <= NumDescriptorsPerHeap && "The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap.");
	}

	void GfxDynamicDescriptorHeap::StageDescriptors(u32 InRootParameterIndex, u32 InOffset, u32 InNumDescriptors, 
		const D3D12_CPU_DESCRIPTOR_HANDLE InSrcDescriptor)
	{
		// Cannot stage more than the maximum number of descriptors per heap.
		// Cannot stage more than MaxDescriptorTables root parameters.
		if (InNumDescriptors > NumDescriptorsPerHeap || InRootParameterIndex >= MaxDescriptorTables)
		{
			throw std::bad_alloc();
		}

		GfxDynamicDescriptorHeap::DescriptorTableCache& descriptorTableCache = DescriptorTableCaches[InRootParameterIndex];

		// Check that the number of descriptors to copy does not exceed the number
		// of descriptors expected in the descriptor table.
		if ((InOffset + InNumDescriptors) > descriptorTableCache.NumDescriptors)
		{
			// NOTE (gb): again this exception stuff..
			throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
		}

		D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.BaseDescriptor + InOffset);
		for (uint32_t i = 0; i < InNumDescriptors; ++i)
		{
			dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(InSrcDescriptor, i, DescriptorHandleIncrementSize);
		}

		// Set the root parameter index bit to make sure the descriptor table 
		// at that index is bound to the command list.
		StaleDescriptorTableBitMask |= (1 << InRootParameterIndex);
	}

	u32 GfxDynamicDescriptorHeap::ComputeStaleDescriptorCount() const
	{
		u32 numStaleDescriptors = 0;
		u64 i;
		u64 staleDescriptorsBitMask = StaleDescriptorTableBitMask;

		while (_BitScanForward(&i, staleDescriptorsBitMask))
		{
			numStaleDescriptors += DescriptorTableCaches[i].NumDescriptors;
			staleDescriptorsBitMask ^= (1 << i);
		}

		return numStaleDescriptors;
	}

	ComPtr<ID3D12DescriptorHeap> GfxDynamicDescriptorHeap::RequestDescriptorHeap()
	{
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		if (!AvailableDescriptorHeaps.empty())
		{
			descriptorHeap = AvailableDescriptorHeaps.front();
			AvailableDescriptorHeaps.pop();
		}
		else
		{
			descriptorHeap = CreateDescriptorHeap();
			DescriptorPool.push(descriptorHeap);
		}

		return descriptorHeap;
	}

	ComPtr<ID3D12DescriptorHeap> GfxDynamicDescriptorHeap::CreateDescriptorHeap()
	{
		auto device = GfxBackend::GetDevice();

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		descriptorHeap = device->CreateDescriptorHeap(DescriptorHeapType, NumDescriptorsPerHeap, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		return descriptorHeap;
	}

	void GfxDynamicDescriptorHeap::CommitStagedDescriptors(GfxCmdList& InCommandList, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> InDelegate)
	{
		// Compute the number of descriptors that need to be copied 
		u32 numDescriptorsToCommit = ComputeStaleDescriptorCount();

		if (numDescriptorsToCommit > 0)
		{
			auto device = GfxBackend::GetDevice()->GetD3D12Device();
			auto d3d12GraphicsCommandList = InCommandList.GetGraphicsCommandList().Get();
			assert(d3d12GraphicsCommandList != nullptr);

			if (!CurrentDescriptorHeap || NumFreeHandles < numDescriptorsToCommit)
			{
				CurrentDescriptorHeap = RequestDescriptorHeap();
				CurrentCPUDescriptorHandle = CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				CurrentGPUDescriptorHandle = CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
				NumFreeHandles = NumDescriptorsPerHeap;

				InCommandList.SetDescriptorHeap(DescriptorHeapType, CurrentDescriptorHeap.Get());

				// When updating the descriptor heap on the command list, all descriptor
				// tables must be (re)recopied to the new descriptor heap (not just
				// the stale descriptor tables).
				StaleDescriptorTableBitMask = DescriptorTableBitMask;
			}

			DWORD rootIndex;
			// Scan from LSB to MSB for a bit set in staleDescriptorsBitMask
			while (_BitScanForward(&rootIndex, StaleDescriptorTableBitMask))
			{
				u64 numSrcDescriptors = DescriptorTableCaches[rootIndex].NumDescriptors;
				D3D12_CPU_DESCRIPTOR_HANDLE* srcDescriptorHandles = DescriptorTableCaches[rootIndex].BaseDescriptor;

				D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStarts[] =
				{
					CurrentCPUDescriptorHandle
				};
				UINT pDestDescriptorRangeSizes[] =
				{
					numSrcDescriptors
				};

				// Copy the staged CPU visible descriptors to the GPU visible descriptor heap.
				device->CopyDescriptors(1, destDescriptorRangeStarts, pDestDescriptorRangeSizes,
					numSrcDescriptors, srcDescriptorHandles, nullptr, DescriptorHeapType);

				// Set the descriptors on the command list using the passed-in setter function.
				InDelegate(d3d12GraphicsCommandList, rootIndex, CurrentGPUDescriptorHandle);

				// Offset current CPU and GPU descriptor handles.
				CurrentCPUDescriptorHandle.Offset(numSrcDescriptors, DescriptorHandleIncrementSize);
				CurrentGPUDescriptorHandle.Offset(numSrcDescriptors, DescriptorHandleIncrementSize);
				NumFreeHandles -= numSrcDescriptors;

				// Flip the stale bit so the descriptor table is not recopied again unless it is updated with a new descriptor.
				StaleDescriptorTableBitMask ^= (1 << rootIndex);
			}
		}
	}

	void GfxDynamicDescriptorHeap::CommitStagedDescriptorsForDraw(GfxCmdList& InCommandList)
	{
		CommitStagedDescriptors(InCommandList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
	}

	void GfxDynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(GfxCmdList& InCommandList)
	{
		CommitStagedDescriptors(InCommandList, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GfxDynamicDescriptorHeap::CopyDescriptor(GfxCmdList& InComandList, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor)
	{
		if (!CurrentDescriptorHeap || NumFreeHandles < 1)
		{
			CurrentDescriptorHeap = RequestDescriptorHeap();
			CurrentCPUDescriptorHandle = CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			CurrentGPUDescriptorHandle = CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			NumFreeHandles = NumDescriptorsPerHeap;

			InComandList.SetDescriptorHeap(DescriptorHeapType, CurrentDescriptorHeap.Get());

			// When updating the descriptor heap on the command list, all descriptor
			// tables must be (re)recopied to the new descriptor heap (not just
			// the stale descriptor tables).
			StaleDescriptorTableBitMask = DescriptorTableBitMask;
		}

		auto device = GfxBackend::GetDevice()->GetD3D12Device();

		D3D12_GPU_DESCRIPTOR_HANDLE hGPU = CurrentGPUDescriptorHandle;
		device->CopyDescriptorsSimple(1, CurrentCPUDescriptorHandle, cpuDescriptor, DescriptorHeapType);

		CurrentCPUDescriptorHandle.Offset(1, DescriptorHandleIncrementSize);
		CurrentGPUDescriptorHandle.Offset(1, DescriptorHandleIncrementSize);
		NumFreeHandles -= 1;

		return hGPU;
	}

	void GfxDynamicDescriptorHeap::Reset()
	{
		AvailableDescriptorHeaps = DescriptorPool;
		CurrentDescriptorHeap.Reset();
		CurrentCPUDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		CurrentGPUDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
		NumFreeHandles = 0;
		DescriptorTableBitMask = 0;
		StaleDescriptorTableBitMask = 0;

		// Reset the table cache
		for (int i = 0; i < MaxDescriptorTables; ++i)
		{
			DescriptorTableCaches[i].Reset();
		}
	}
}