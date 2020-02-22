#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/Base/inc_gfx_types.h"

namespace Dawn 
{
	class GfxGDI;

	namespace ImGuiWrapper
	{
		void Create(void* InHwnd, GfxGDI* InGDI);
		void Shutdown();
		void BeginNewFrame();
		void Render();
		LRESULT InputHandling(HWND InHwnd, UINT InMsg, WPARAM InWParam, LPARAM InLParam);
		void Resize();
	}
}