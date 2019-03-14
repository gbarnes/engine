#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "../../inc_common.h"
#include <Windows.h>
#include <wrl/client.h>
#include "inc_gfx.h"

namespace Dawn 
{
	const u8 g_NumFrames = 3;

	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// The GrahpicsDevice is used to setup DirectX12 and provide usage of it!
	//-----------------------------------------------------------------------------
	class CGfxDevice
	{
	public:
		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		CGfxDevice::CGfxDevice();
		CGfxDevice::~CGfxDevice(void);

		//-------------------------------------------------------------------------
		// Public Members
		//-------------------------------------------------------------------------
		EResult CGfxDevice::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false);
		void CGfxDevice::Present();
		void CGfxDevice::Shutdown();

		const u32 CGfxDevice::GetCurrentBufferIndex() const {
			return BackBufferIndex;
		}

		ComPtr<CGfxQueue> CGfxDevice::GetBackbufferQueue() const {
			return this->BackbufferQueue;
		}

		ComPtr<CGfxResource> CGfxDevice::GetBackbuffer() const {
			return this->BackBufferRenderTarget[BackBufferIndex];
		}

		ComPtr<CGfxHeapDesciptor> CGfxDevice::GetRTVHeap() const {
			return this->RenderTargetViewHeap;
		}

		ComPtr<ID3D12Device> CGfxDevice::GetD3D12Device() const {
			return this->Device;
		}

		u32 CGfxDevice::GetRTVDescriptorSize() const {
			return this->RTVDescriptorSize;
		}

		ComPtr<CGfxQueue> CGfxDevice::CreateQueue(D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxHeapDesciptor> CGfxDevice::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		ComPtr<CGfxCmdAllocator> CGfxDevice::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxCmdList> CGfxDevice::CreateCmdList(ComPtr<CGfxCmdAllocator> InAllocator, D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxFence> CGfxDevice::CreateFence();



	private:
		ComPtr<ID3D12Device> Device;
		ComPtr<IDXGISwapChain3> SwapChain;
		ComPtr<CGfxHeapDesciptor> RenderTargetViewHeap;
		ComPtr<CGfxResource> BackBufferRenderTarget[g_NumFrames];
		ComPtr<CGfxQueue> BackbufferQueue;

		u32 RTVDescriptorSize;
		bool bUseVSync;
		bool bIsFullscreen;
		bool bIsTearingSupported;
		u32 BackBufferIndex;

		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
		void CGfxDevice::EnableDebugLayer();
		bool CGfxDevice::CheckForTearingSupport();
		void CGfxDevice::UpdateRenderTargetViews();

		ComPtr<ID3D12Device> CGfxDevice::CreateDevice(ComPtr<IDXGIAdapter3> InAdapter);
		ComPtr<IDXGIAdapter3> CGfxDevice::GetAdapter(bool InUseWrap);
		ComPtr<IDXGISwapChain3> CGfxDevice::CreateSwapChain(HWND InHwnd, ComPtr<CGfxQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount);
	};
}
