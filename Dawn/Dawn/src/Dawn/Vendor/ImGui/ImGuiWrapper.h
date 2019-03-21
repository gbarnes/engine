#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn 
{
	namespace ImGuiWrapper
	{
		void Create(void* InHwnd);
		void Shutdown();
		void BeginNewFrame();
		void Render(ID3D12GraphicsCommandList2* InCmdList);
		LRESULT InputHandling(HWND InHwnd, UINT InMsg, WPARAM InWParam, LPARAM InLParam);
		void Resize();
	}
}