#pragma once
#ifdef USE_DX12_GFX
#include "inc_gfx_types.h"
#include <functional>
#include <queue>

namespace Dawn
{
	class GfxCmdList;
	class GfxRootSignature;
	typedef std::queue< ComPtr<ID3D12DescriptorHeap> > DescriptorHeapPool;

	//
	// Class taken from the 3dgep.com tutorial series!
	// See https://www.3dgep.com/learning-directx12-3/
	// I might change this once I better grasped the concept behind DX12
	// and find a better way of putting the concept of dx12 into a layer.
	//
	class GfxDynamicDescriptorHeap
	{
	public:
		GfxDynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptorsPerHeap = 1024);
		virtual ~GfxDynamicDescriptorHeap();

		void StageDescriptors(u32 InRootParameterIndex, u32 InOffset, 
			u32 InNumDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE InSrcDescriptors);

		void CommitStagedDescriptors(GfxCmdList& InCmdList, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> InDelegate);
		void CommitStagedDescriptorsForDraw(GfxCmdList& InCmdList);
		void CommitStagedDescriptorsForDispatch(GfxCmdList& InCmdList);

		D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(GfxCmdList& InCmdList, D3D12_CPU_DESCRIPTOR_HANDLE InCpuDescriptor);
		void ParseRootSignature(const GfxRootSignature& InRootSignature);
		void Reset();
	private:
		ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

		u32 ComputeStaleDescriptorCount() const;

		/**
		 * The maximum number of descriptor tables per root signature.
		 * A 32-bit mask is used to keep track of the root parameter indices that
		 * are descriptor tables.
		 */
		static const u32 MaxDescriptorTables = 32;

		struct DescriptorTableCache
		{
			DescriptorTableCache() 
				: NumDescriptors(0)
				, BaseDescriptor(nullptr)
			{}

			void Reset()
			{
				NumDescriptors = 0;
				BaseDescriptor = nullptr;
			}

			u32 NumDescriptors;
			D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
		};

		u32 NumFreeHandles;
		u32 NumDescriptorsPerHeap;
		u32 DescriptorHandleIncrementSize;
		u32 DescriptorTableBitMask;
		u32 StaleDescriptorTableBitMask;

		D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType;
		std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> DescriptorHandleCache;
		DescriptorTableCache DescriptorTableCaches[MaxDescriptorTables];
		
		DescriptorHeapPool DescriptorPool;
		DescriptorHeapPool AvailableDescriptorHeaps;
		ComPtr<ID3D12DescriptorHeap> CurrentDescriptorHeap;
		CD3DX12_GPU_DESCRIPTOR_HANDLE CurrentGPUDescriptorHandle;
		CD3DX12_CPU_DESCRIPTOR_HANDLE CurrentCPUDescriptorHandle;
	};

}
#endif