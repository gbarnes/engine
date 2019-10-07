#include "GfxImmediatePrimitives.h"
#include "OpenGL/GLImmediatePrimitives.h"
#include "EntitySystem/Camera/Camera.h"
#include "GfxGDI.h"
#include "inc_common.h"

namespace Dawn
{
	GfxImmediatePrimitives::GfxImmediatePrimitives(Shared<GfxGDI> InGDI)
	{
		this->GDI = InGDI;
	}
}