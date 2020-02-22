#include "stdafx.h"
#include "GfxImmediatePrimitives.h"
#include "OpenGL/GLImmediatePrimitives.h"
#include "Base/GfxGDI.h"

namespace Dawn
{
	GfxImmediatePrimitives::GfxImmediatePrimitives(Shared<GfxGDI> InGDI)
	{
		this->GDI = InGDI;
	}
}