#pragma once
#ifdef USE_DX12_GFX
#include "inc_common.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	namespace GfxContext
	{
		ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter3> InAdapter);
		ComPtr<IDXGIAdapter3> GetAdapter(bool InUseWrap);
		ComPtr<IDXGISwapChain3> CreateSwapChain(HWND InHwnd, ComPtr<ID3D12CommandQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount);
		bool CheckForTearingSupport();
		bool SupportsMath();
	}
}
#endif