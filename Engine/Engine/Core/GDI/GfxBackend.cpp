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

CGfxDevice g_Device;
HANDLE g_FenceEvent;
ComPtr<CGfxFence> g_Fence;
ComPtr<CGfxCmdList> g_CommandList;
ComPtr<CGfxCmdAllocator> g_CommandAllocators[g_NumFrames];
u64 g_FrameFenceValues[g_NumFrames] = {};
u64 g_FenceValue = 0;
bool g_Initialized = false;

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
	g_Device.Shutdown();
}

void GfxBackend::ClearBackbuffer(DirectX::XMFLOAT4 InColor)
{
	// Clear the render target.
	{
		u32 BackBufferIndex = g_Device.GetCurrentBufferIndex();
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			g_Device.GetBackbuffer().Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		g_CommandList->ResourceBarrier(1, &barrier);

		FLOAT clearColor[] = { InColor.x, InColor.y, InColor.z, InColor.w };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			g_Device.GetRenderTargetViewHeap()->GetCPUDescriptorHandleForHeapStart(),
			BackBufferIndex,
			g_Device.GetRTVDescriptorSize()
		);

		g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}
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
}

//
//
//
void GfxBackend::Present()
{
	u32 BackBufferIndex = g_Device.GetCurrentBufferIndex();

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		g_Device.GetBackbuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
	);

	g_CommandList->ResourceBarrier(1, &barrier);

	ThrowIfFailed(g_CommandList->Close());

	ID3D12CommandList* const commandLists[] = 
	{
		g_CommandList.Get()
	};

	g_Device.GetBackbufferQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

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
void GfxBackend::WaitForFenceValue(ComPtr<CGfxFence> InFence, u64 InFenceValue, HANDLE InFenceEvent, std::chrono::milliseconds InDuration )
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