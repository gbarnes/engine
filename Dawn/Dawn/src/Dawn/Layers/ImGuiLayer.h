#pragma once

#include "Layer.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	class CImGuiLayer : public CLayer
	{
	public:
		CImGuiLayer(HWND InWindowHandle);
		~CImGuiLayer();
		
		void Setup();
		void Update();
		void Render(ComPtr<CGfxCmdList> InCmdList);
		void Free();

	private:
		HWND WindowHandle;
	};

};
