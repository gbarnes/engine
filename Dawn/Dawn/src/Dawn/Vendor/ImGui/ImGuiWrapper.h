#pragma once
#include "inc_common.h"
#include "inc_core.h"

namespace Dawn 
{
	namespace ImGuiWrapper
	{
		void Create(void* InHwnd);
		void Shutdown();
		void BeginNewFrame();
		void Render();
	}
}