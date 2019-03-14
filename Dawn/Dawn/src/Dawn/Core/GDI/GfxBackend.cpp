#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"
#include "GfxDevice.h"
#include "GfxParallel.h"
#include "GfxBackend.h"
#include "imgui.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"

namespace Dawn
{
	CGfxDevice g_Device;
	HANDLE g_FenceEvent;
	ComPtr<CGfxFence> g_Fence;
	ComPtr<CGfxCmdList> g_CommandList;
	ComPtr<CGfxCmdAllocator> g_CommandAllocators[g_NumFrames];
	u64 g_FrameFenceValues[g_NumFrames] = {};
	u64 g_FenceValue = 0;
	bool g_Initialized = false;

	D3D12_RESOURCE_STATES g_LastResourceTransitionState = D3D12_RESOURCE_STATE_PRESENT;

	inline HANDLE CreateEventHandle()
	{
		HANDLE FenceEvent;

		FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(FenceEvent && "Failed to create fence event.");

		return FenceEvent;
	}

	

	bool GfxBackend::IsInitialized()
	{
		return g_Initialized;
	}

	EResult GfxBackend::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false)
	{
		if (g_Device.Initialize(InWidth, InHeight, InHWnd, InUseVSync, InIsFullscreen) != EResult_OK)
			return EResult_ERROR;

		for (int i = 0; i < g_NumFrames; ++i)
		{
			g_CommandAllocators[i] = g_Device.CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT);
		}

		g_CommandList = g_Device.CreateCmdList(g_CommandAllocators[g_Device.GetCurrentBufferIndex()], D3D12_COMMAND_LIST_TYPE_DIRECT);

		g_Fence = g_Device.CreateFence();
		g_FenceEvent = CreateEventHandle();

		g_Initialized = true;

		return EResult_OK;
	}

	//
	//
	//
	void GfxBackend::Shutdown()
	{
		Flush(g_Device.GetBackbufferQueue(), g_Fence, g_FenceValue, g_FenceEvent);
		::CloseHandle(g_FenceEvent);

		g_CommandList->Close();
		g_CommandList.Reset();

		for (int i = 0; i < g_NumFrames; ++i)
		{
			g_CommandAllocators[i].Reset();
		}

		g_Fence.Reset();
		g_Device.Shutdown();
	}

	void GfxBackend::ClearRenderTarget(ComPtr<CGfxResource> InRenderTarget, DirectX::XMFLOAT4 InColor)
	{
		FLOAT clearColor[] = { InColor.x, InColor.y, InColor.z, InColor.w };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}

	void GfxBackend::TransitionResource(ComPtr<CGfxResource> InResource, D3D12_RESOURCE_STATES InPreviousState, D3D12_RESOURCE_STATES InState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			InResource.Get(),
			InPreviousState,
			InState,
			D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		);

		g_CommandList->ResourceBarrier(1, &barrier);
	}

	//
	//
	//
	void GfxBackend::Reset()
	{
		u32 BackBufferIndex = g_Device.GetCurrentBufferIndex();
		auto commandAllocator = g_CommandAllocators[BackBufferIndex];
		commandAllocator->Reset();
		g_CommandList->Reset(commandAllocator.Get(), nullptr);
		TransitionResource(g_Device.GetBackbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}

	//
	//
	//
	void GfxBackend::Present()
	{
		
		TransitionResource(GfxBackend::GetCurrentBackbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		ThrowIfFailed(g_CommandList->Close());

		ID3D12CommandList* const commandLists[] = {
			g_CommandList.Get()
		};

		g_Device.GetBackbufferQueue()->ExecuteCommandLists(1, commandLists);

		u32 BackBufferIndex = g_Device.GetCurrentBufferIndex();
		g_FrameFenceValues[BackBufferIndex] = SignalFence(g_Device.GetBackbufferQueue(), g_Fence, g_FenceValue);
		g_Device.Present();
		GfxBackend::WaitForFenceValue(g_Fence, g_FrameFenceValues[BackBufferIndex], g_FenceEvent);
	}

	//
	// 
	//
	u64 GfxBackend::SignalFence(ComPtr<CGfxQueue> InQueue, ComPtr<CGfxFence> InFence, u64& InFenceValue)
	{
		u64 FenceValueForSignal = ++InFenceValue;
		ThrowIfFailed(InQueue->Signal(InFence.Get(), FenceValueForSignal));

		return FenceValueForSignal;
	}

	//
	//
	//
	void GfxBackend::WaitForFenceValue(ComPtr<CGfxFence> InFence, u64 InFenceValue, HANDLE InFenceEvent, std::chrono::milliseconds InDuration)
	{
		if (InFence->GetCompletedValue() < InFenceValue)
		{
			ThrowIfFailed(InFence->SetEventOnCompletion(InFenceValue, InFenceEvent));
			::WaitForSingleObject(InFenceEvent, static_cast<DWORD>(InDuration.count()));
		}
	}

	void GfxBackend::Flush(ComPtr<CGfxQueue> InCommandQueue, ComPtr<CGfxFence> InFence, u64& InFenceValue, HANDLE InFenceEvent)
	{
		u64 FenceValueForSignal = SignalFence(InCommandQueue, InFence, InFenceValue);
		WaitForFenceValue(InFence, FenceValueForSignal, InFenceEvent);
	}
	
	CGfxDevice* GfxBackend::GetDevice()
	{
		return &g_Device;
	}

	ComPtr<CGfxCmdList> GfxBackend::GetCommandList()
	{
		return g_CommandList;
	}

	ComPtr<CGfxResource> GfxBackend::GetCurrentBackbuffer()
	{
		return g_Device.GetBackbuffer();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GfxBackend::GetCurrentBackbufferDescHandle()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
		(
			g_Device.GetRTVHeap()->GetCPUDescriptorHandleForHeapStart(),
			g_Device.GetCurrentBufferIndex(),
			g_Device.GetRTVDescriptorSize()
		);

		return rtv;
	}

}