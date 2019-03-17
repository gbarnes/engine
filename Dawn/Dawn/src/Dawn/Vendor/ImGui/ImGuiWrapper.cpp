#include "ImGuiWrapper.h"
#include "imgui.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"

#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxDevice.h"

namespace Dawn
{
	ComPtr<CGfxHeapDesciptor> g_D3DSrvDescHeap;

	void ImGuiWrapper::Create(void* InHwnd)
	{
		// Todo: Add error checking & Logs 
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		//io.DisplaySize = ImVec2(1280, 720);
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		CGfxDevice* Device = GfxBackend::GetDevice();
		if (Device == nullptr)
		{
			DWN_CORE_ERROR("There is no available graphics device!");
			return;
		}


		g_D3DSrvDescHeap = Device->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

		ImGui_ImplWin32_Init(InHwnd);
		ImGui_ImplDX12_Init
		(
			Device->GetD3D12Device().Get(),
			g_NumFrames,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			g_D3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_D3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
	}

	void ImGuiWrapper::Shutdown()
	{
		g_D3DSrvDescHeap.Reset();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiWrapper::BeginNewFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiWrapper::Resize()
	{
		ImGui_ImplDX12_CreateDeviceObjects();
	}

	void ImGuiWrapper::Render(CGfxCmdList* InCmdList)
	{
		InCmdList->SetDescriptorHeaps(1, g_D3DSrvDescHeap.GetAddressOf());

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), InCmdList);
	}
}