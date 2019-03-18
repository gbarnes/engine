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

	class GfxQueue;

	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// The GrahpicsDevice is used to setup DirectX12 and provide usage of it!
	//-----------------------------------------------------------------------------
	class GfxDevice
	{
	public:
		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		GfxDevice::GfxDevice();
		GfxDevice::~GfxDevice(void);

		//-------------------------------------------------------------------------
		// Public Members
		//-------------------------------------------------------------------------
		EResult GfxDevice::Initialize(u32 InWidth, u32 InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false);
		u64 GfxDevice::Present();
		void GfxDevice::Shutdown();

		const u32 GfxDevice::GetCurrentBufferIndex() const {
			return BackBufferIndex;
		}

		std::shared_ptr<GfxQueue> GfxDevice::GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const {

			std::shared_ptr<GfxQueue> CommandQueue;
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

		ComPtr<CGfxResource> GfxDevice::GetBackbuffer() const {
			return this->BackBufferRenderTarget[BackBufferIndex];
		}

		ComPtr<CGfxHeapDesciptor> GfxDevice::GetRTVHeap() const {
			return this->RenderTargetViewHeap;
		}

		ComPtr<CGfxResource> GfxDevice::GetDepthBuffer() const {
			return this->DepthBuffer;
		}

		ComPtr<CGfxHeapDesciptor> GfxDevice::GetDSVHeap() const {
			return this->DepthBufferHeap;
		}

		ComPtr<ID3D12Device2> GfxDevice::GetD3D12Device() const {
			return this->Device;
		}

		CD3DX12_VIEWPORT GfxDevice::GetViewport() const {
			return this->Viewport;
		}

		CD3DX12_RECT GfxDevice::GetScissorRect() const {
			return this->ScissorRect;
		}

		u32 GfxDevice::GetRTVDescriptorSize() const {
			return this->RTVDescriptorSize;
		}

		ComPtr<CGfxHeapDesciptor> GfxDevice::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS InFlags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		ComPtr<CGfxCmdAllocator> GfxDevice::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxCmdList> GfxDevice::CreateCmdList(ComPtr<CGfxCmdAllocator> InAllocator, D3D12_COMMAND_LIST_TYPE InType);
		ComPtr<CGfxFence> GfxDevice::CreateFence();
		void GfxDevice::CreateDepthBuffer(u32 InWidth, u32 InHeight);

		void GfxDevice::Resize(u32 InWidth, u32 InHeight);
		
	private:
		ComPtr<ID3D12Device2> Device;
		ComPtr<IDXGISwapChain3> SwapChain;

		// Depth buffer.
		ComPtr<CGfxResource> DepthBuffer;
		ComPtr<CGfxHeapDesciptor> DepthBufferHeap;

		ComPtr<CGfxHeapDesciptor> RenderTargetViewHeap;
		ComPtr<CGfxResource> BackBufferRenderTarget[g_NumFrames];

		std::shared_ptr<GfxQueue> DirectCommandQueue;
		std::shared_ptr<GfxQueue> CopyCommandQueue;
		std::shared_ptr<GfxQueue> ComputeCommandQueue;

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
		void GfxDevice::EnableDebugLayer();
		bool GfxDevice::CheckForTearingSupport();
		void GfxDevice::UpdateRenderTargetViews();

		ComPtr<ID3D12Device2> GfxDevice::CreateDevice(ComPtr<IDXGIAdapter3> InAdapter);
		ComPtr<IDXGIAdapter3> GfxDevice::GetAdapter(bool InUseWrap);
		ComPtr<IDXGISwapChain3> GfxDevice::CreateSwapChain(HWND InHwnd, ComPtr<GfxInternalQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount);
	};
}
