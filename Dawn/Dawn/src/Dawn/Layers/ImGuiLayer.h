#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	class GfxCmdList;

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(HWND InWindowHandle);
		~ImGuiLayer();
		
		void Setup();
		void Update();
		void Render(GfxCmdList* InCmdList);
		void Free();

	private:
		HWND WindowHandle;
	};

};
