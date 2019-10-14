#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	class Application;

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(const Shared<Dawn::Application>& InApplication, HWND InWindowHandle);
		~ImGuiLayer();
		
		void Setup();
		void Update(float InDeltaTime);
		void Render();
		void Process(float InDeltaTime);
		void Free();

	private:
		HWND WindowHandle;
	};

};
