#include "stdafx.h"
#include "ImGuiWrapper.h"
#include "imgui.h"
#include "Core/GDI/Base/GfxGDI.h"
#include "Vendor/ImGui/imgui_impl_win32.h"

#define USE_DX11_GFX

#ifdef USE_DX11_GFX
#include "Core/GDI/DX11/DX11GDI.h"
#include "Vendor/ImGui/imgui_impl_dx11.h"
#endif

#ifdef USE_OPENGL_GFX
#include "Core/GDI/OpenGL/GfxGLGDI.h"
#include "Vendor/ImGui/imgui_impl_opengl3.h"
#endif

#ifdef USE_DX12_GFX
#include "Vendor/ImGui/imgui_impl_dx12.h"
#include "Core/GDI/GfxBackend.h"
#endif

namespace Dawn
{
#ifdef USE_DX12_GFX
	ComPtr<ID3D12DescriptorHeap> g_D3DSrvDescHeap;
#endif

	
	static HWND g_hwnd = 0;
	static HGLRC g_glcontext = 0;

	void ImGuiWrapper::Create(void* InHwnd, GfxGDI* InGDI)
	{
		DX11GDI* gdi = static_cast<DX11GDI*>(InGDI);

		// Todo: Add error checking & Logs 
		IMGUI_CHECKVERSION();
		auto* context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(1280, 720);
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //| ImGuiConfigFlags_ViewportsEnable;

		g_hwnd = (HWND)InHwnd;
		ImGui_ImplWin32_Init(InHwnd);

#ifdef USE_DX11_GFX
		ImGui_ImplDX11_Init(gdi->GetD3D11Device(), gdi->GetD3D11Context());
#endif

#ifdef USE_OPENGL_GFX
		g_glcontext = (HGLRC)GLGDI->hRC;
		ImGui_ImplOpenGL3_Init("#version 150");
#endif
		

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

#ifdef USE_OPENGL_GFX
		ImGui_ImplOpenGL3_Shutdown();
#endif

#ifdef USE_DX11_GFX
		ImGui_ImplDX11_Shutdown();
#endif

		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiWrapper::BeginNewFrame()
	{
#ifdef USE_DX12_GFX
		ImGui_ImplDX12_NewFrame();
#endif

#ifdef USE_OPENGL_GFX
		ImGui_ImplOpenGL3_NewFrame();
#endif

#ifdef USE_DX11_GFX
		ImGui_ImplDX11_NewFrame();
#endif
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
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();
		
#ifdef USE_DX11_GFX
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#endif

#ifdef USE_OPENGL_GFX
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto hRC = wglGetCurrentContext();
			HDC backup_current_context = GetDC(g_hwnd);

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			wglMakeCurrent(backup_current_context, g_glcontext);
		}
#endif

#ifdef USE_DX12_GFX
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), InCmdList);
#endif
	}
}