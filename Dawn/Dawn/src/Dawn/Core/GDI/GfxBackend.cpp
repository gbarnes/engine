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
	ComPtr<CGfxHeapDesciptor> g_D3DSrvDescHeap;
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

		g_D3DSrvDescHeap.Reset();

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

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
		(
			g_Device.GetRenderTargetViewHeap()->GetCPUDescriptorHandleForHeapStart(),
			g_Device.GetCurrentBufferIndex(),
			g_Device.GetRTVDescriptorSize()
		);

		g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}

	void GfxBackend::TransitionResource(ComPtr<CGfxResource> InResource, D3D12_RESOURCE_STATES InState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			InResource.Get(),
			g_LastResourceTransitionState,
			InState,
			D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		);

		g_CommandList->ResourceBarrier(1, &barrier);
		g_LastResourceTransitionState = InState;
	}

	//
	//
	//
	void GfxBackend::Reset()
	{
		// This is only temporary
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			//ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

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

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			g_Device.GetRenderTargetViewHeap()->GetCPUDescriptorHandleForHeapStart(),
			BackBufferIndex,
			g_Device.GetRTVDescriptorSize()
		);
		
		TransitionResource(g_Device.GetBackbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		ClearRenderTarget(g_Device.GetBackbuffer(), );
		g_CommandList->OMSetRenderTargets(1, &rtv, FALSE, NULL);
		g_CommandList->SetDescriptorHeaps(1, g_D3DSrvDescHeap.GetAddressOf());


		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_CommandList.Get());

		TransitionResource(g_Device.GetBackbuffer(), D3D12_RESOURCE_STATE_PRESENT);

		ThrowIfFailed(g_CommandList->Close());

		ID3D12CommandList* const commandLists[] = {
			g_CommandList.Get()
		};

		g_Device.GetBackbufferQueue()->ExecuteCommandLists(1, commandLists);

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


	void GfxBackend::ImGuiDX12Init() 
	{
		g_D3DSrvDescHeap = g_Device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		ImGui_ImplDX12_Init(g_Device.GetD3D12Device().Get(), 
			g_NumFrames,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			g_D3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_D3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}
}