#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(HWND InWindowHandle);
		~ImGuiLayer();
		
		void Setup();
		void Update();
		void Render();
		void Process();
		void Free();

	private:
		HWND WindowHandle;
	};

};
