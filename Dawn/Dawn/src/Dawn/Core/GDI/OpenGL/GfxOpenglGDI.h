#pragma once
#include "../GfxGDI.h"

//#ifdef USE_OPENGL_GFX
#include "inc_opengl.h"

namespace Dawn
{
	class DAWN_API GfxOpenglGDI : public GfxGDI
	{
	public:
		bool Init(const AppSettings& InSettings);
		void Present();
		void Shutdown();
	private:
		HWND HwnD;
		HDC hDC;
		HGLRC hRC;
	};
}
//#endif