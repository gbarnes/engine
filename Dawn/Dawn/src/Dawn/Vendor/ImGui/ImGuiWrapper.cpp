#include "ImGuiWrapper.h"
#include "imgui.h"
#include "Vendor/ImGui/imgui_impl_win32.h"

#include "Vendor/ImGui/imgui_impl_opengl3.h"

#ifdef USE_DX12_GFX
#include "Vendor/ImGui/imgui_impl_dx12.h"
#include "Core/GDI/GfxBackend.h"
#endif

namespace Dawn
{
#ifdef USE_DX12_GFX
	ComPtr<ID3D12DescriptorHeap> g_D3DSrvDescHeap;
#endif

	void ImGuiWrapper::Create(void* InHwnd)
	{
		// Todo: Add error checking & Logs 
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(1280, 720);
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;


		ImGui_ImplWin32_Init(InHwnd);
		ImGui_ImplOpenGL3_Init("#version 150");


#ifdef USE_DX12_GFX
		auto Device = GfxBackend::GetDevice();
		if (Device == nullptr)
		{
			DWN_CORE_ERROR("There is no available graphics device!");
			return;
		}

		D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
		Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		Desc.NumDescriptors = 1;
		Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&g_D3DSrvDescHeap)));

		ImGui_ImplDX12_Init
		(
			Device.Get(),
			3,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			g_D3DSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_D3DSrvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);
#endif
	}

	void ImGuiWrapper::Shutdown()
	{
#ifdef USE_DX12_GFX
		g_D3DSrvDescHeap.Reset();
		ImGui_ImplDX12_Shutdown();
#endif

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiWrapper::BeginNewFrame()
	{
#ifdef USE_DX12_GFX
		ImGui_ImplDX12_NewFrame();
#endif
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	LRESULT ImGuiWrapper::InputHandling(HWND InHwnd, UINT InMsg, WPARAM InWParam, LPARAM InLParam)
	{
		if (ImGui::GetCurrentContext() == NULL)
			return 0;

		ImGuiIO& io = ImGui::GetIO();
		switch (InMsg)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			int button = 0;
			if (InMsg == WM_LBUTTONDOWN || InMsg == WM_LBUTTONDBLCLK) { button = 0; }
			if (InMsg == WM_RBUTTONDOWN || InMsg == WM_RBUTTONDBLCLK) { button = 1; }
			if (InMsg == WM_MBUTTONDOWN || InMsg == WM_MBUTTONDBLCLK) { button = 2; }
			if (InMsg == WM_XBUTTONDOWN || InMsg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(InWParam) == XBUTTON1) ? 3 : 4; }
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
				::SetCapture(InHwnd);
			io.MouseDown[button] = true;
			return 0;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			int button = 0;
			if (InMsg == WM_LBUTTONUP) { button = 0; }
			if (InMsg == WM_RBUTTONUP) { button = 1; }
			if (InMsg == WM_MBUTTONUP) { button = 2; }
			if (InMsg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(InWParam) == XBUTTON1) ? 3 : 4; }
			io.MouseDown[button] = false;
			if (!ImGui::IsAnyMouseDown() && ::GetCapture() == InHwnd)
				::ReleaseCapture();
			return 0;
		}
		case WM_MOUSEWHEEL:
			io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(InWParam) / (float)WHEEL_DELTA;
			return 0;
		case WM_MOUSEHWHEEL:
			io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(InWParam) / (float)WHEEL_DELTA;
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (InWParam < 256)
				io.KeysDown[InWParam] = 1;
			return 0;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if (InWParam < 256)
				io.KeysDown[InWParam] = 0;
			return 0;
		case WM_CHAR:
			// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
			if (InWParam > 0 && InWParam < 0x10000)
				io.AddInputCharacter((unsigned short)InWParam);
			return 0;
		
		
		}
		return 0;
	}

	void ImGuiWrapper::Resize()
	{
#ifdef USE_DX12_GFX
		ImGui_ImplDX12_CreateDeviceObjects();
#endif
	}

	void ImGuiWrapper::Render()
	{
#ifdef USE_DX12_GFX
		InCmdList->SetDescriptorHeaps(1, g_D3DSrvDescHeap.GetAddressOf());
#endif

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#ifdef USE_DX12_GFX
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), InCmdList);
#endif
	}
}