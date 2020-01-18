#include "stdafx.h"
#include "GfxImmediatePrimitives.h"
#include "OpenGL/GLImmediatePrimitives.h"
#include "GfxGDI.h"

namespace Dawn
{
	GfxImmediatePrimitives::GfxImmediatePrimitives(Shared<GfxGDI> InGDI)
	{
		this->GDI = InGDI;
	}
}