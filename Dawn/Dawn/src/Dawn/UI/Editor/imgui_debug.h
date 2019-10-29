#pragma once
#include "inc_core.h"

class GfxRenderBuffer;

namespace Dawn
{
	void ShowGBuffer(GfxRenderBuffer* InBuffer, GfxRenderBuffer* InShadowBuffer);
	void ShowFpsCounter();
}