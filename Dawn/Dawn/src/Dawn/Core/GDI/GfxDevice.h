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

	class CGfxQueue;

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
		EResult CGfxDevice::Initialize(u32 InWidth, u32 InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false);
		u64 CGfxDevice::Present();
		void CGfxDevice::Shutdown();

		const u32 CGfxDevice::GetCurrentBufferIndex() const {
			return BackBufferIndex;
		}

		std::shared_ptr<CGfxQueue> CGfxDevice::GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const {

			std::shared_ptr<CGfxQueue> CommandQueue;
			switch (type)
			{
			case D3D12_COMMAND_LIST_TYPE_DIRECT:
				CommandQueue = this->DirectCommandQueue;
				break;
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:
				CommandQueue = ComputeCommandQueue;
				break;
			case D3D12_COMMAND_LIST_TYPE_COPY:
				CommandQueue = CopyCommandQueue;
				break;
			default:
				assert(false && "Invalid command queue type.");
			}

			return CommandQueue;
		}

		ComPtr<CGfxResource> CGfxDevice::GetBackbuffer() const {
			return this->BackBufferRenderTarget[BackBufferIndex];
		}

		ComPtr<CGfxHeapDesciptor> CGfxDevice::GetRTVHeap() const {
			return this->RenderTargetViewHeap;
		}

		ComPtr<CGfxResource> CGfxDevice::GetDepthBuffer() const {
			return this->DepthBuffer;
		}

		ComPtr<CGfxHeapDesciptor> CGfxDevice::GetDSVHeap() const {
			return this->DepthBufferHeap;
		}

		ComPtr<ID3D12Device2> CGfxDevice::GetD3D12Device() const {
			return this->Device;
		}

		CD3DX12_VIEWPORT CGfxDevice::GetViewport() const {
			return this->Viewport;
		}

		CD3DX12_RECT CGfxDevice::GetScissorRect() const {
			return this->ScissorRect;
		}

		u32 CGfxDevice::GetRTVDescriptorSize() const {
			return this->RTVDescriptorSize;
		}

		ComPtr<CGfxHeapDesciptor> CGfxDevice::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		ComPtr<CGfxCmdAllocator> CGfxDevice::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxCmdList> CGfxDevice::CreateCmdList(ComPtr<CGfxCmdAllocator> InAllocator, D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxFence> CGfxDevice::CreateFence();
		void CGfxDevice::CreateDepthBuffer(u32 InWidth, u32 InHeight);

		void CGfxDevice::Resize(u32 InWidth, u32 InHeight);
		
	private:
		ComPtr<ID3D12Device2> Device;
		ComPtr<IDXGISwapChain3> SwapChain;

		// Depth buffer.
		ComPtr<CGfxResource> DepthBuffer;
		ComPtr<CGfxHeapDesciptor> DepthBufferHeap;

		ComPtr<CGfxHeapDesciptor> RenderTargetViewHeap;
		ComPtr<CGfxResource> BackBufferRenderTarget[g_NumFrames];

		std::shared_ptr<CGfxQueue> DirectCommandQueue;
		std::shared_ptr<CGfxQueue> CopyCommandQueue;
		std::shared_ptr<CGfxQueue> ComputeCommandQueue;

		CD3DX12_VIEWPORT Viewport;
		CD3DX12_RECT ScissorRect;

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

		ComPtr<ID3D12Device2> CGfxDevice::CreateDevice(ComPtr<IDXGIAdapter3> InAdapter);
		ComPtr<IDXGIAdapter3> CGfxDevice::GetAdapter(bool InUseWrap);
		ComPtr<IDXGISwapChain3> CGfxDevice::CreateSwapChain(HWND InHwnd, ComPtr<CGfxInternalQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount);
	};
}
