#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "inc_common.h"

namespace Dawn 
{
	//-----------------------------------------------------------------------------
	// Typedefs
	//-----------------------------------------------------------------------------
	typedef ID3D12CommandQueue CGfxQueue;
	typedef D3D12_COMMAND_QUEUE_DESC CGfxQueueDesc;
	typedef ID3D12CommandAllocator CGfxCmdAllocator;
	typedef ID3D12GraphicsCommandList CGfxCmdList;
	typedef ID3D12Fence CGfxFence;
	typedef D3D12_RESOURCE_BARRIER SGfxResBarrier;
	typedef ID3D12Resource CGfxResource;
	typedef ID3D12PipelineState CGfxState;
	typedef ID3D12DescriptorHeap CGfxHeapDesciptor;

	typedef SGenericHandle<12, 20> SGfxResourceHandle;
	typedef SGenericHandle<12, 20> SVertexBufferHandle;
	typedef SGenericHandle<12, 20> SIndexBufferHandle;
	typedef SGenericHandle<12, 20> SConstantBufferHandle;
	typedef SGenericHandle<12, 20> SPipelineStateHandle;
	typedef SGenericHandle<12, 20> SCmdListHandle;
	typedef SGenericHandle<12, 20> SFenceHandle;

	namespace GfxBackend 
	{
		EResult Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync, bool InIsFullscreen);

		bool IsInitialized();

		void Shutdown();
		void Reset();
		void ClearRenderTarget(ComPtr<CGfxResource> InRenderTarget, DirectX::XMFLOAT4 InColor);
		void Present();

		//-------------------------------------------------------------------------
		// DX12 Fence Handling
		//-------------------------------------------------------------------------
		u64 SignalFence(ComPtr<CGfxQueue> InQueue, ComPtr<CGfxFence> InFence, u64& InFenceValue);
		void WaitForFenceValue(ComPtr<CGfxFence> InFence, u64 InFenceValue, HANDLE InFenceEvent, std::chrono::milliseconds InDuration = std::chrono::milliseconds::max());
		void Flush(ComPtr<CGfxQueue> InCommandQueue, ComPtr<CGfxFence> InFence, u64& InFenceValue, HANDLE InFenceEvent);

		void TransitionResource(ComPtr<CGfxResource> InResource, D3D12_RESOURCE_STATES InState);


		void ImGuiDX12Init();
	}
};