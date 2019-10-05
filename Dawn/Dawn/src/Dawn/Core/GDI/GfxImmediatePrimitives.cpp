#include "GfxImmediatePrimitives.h"
#include "OpenGL/GLImmediatePrimitives.h"
#include "EntitySystem/Camera/Camera.h"
#include "inc_common.h"

namespace Dawn
{
	GfxImmediatePrimitives* GfxImmediatePrimitives::Instance = nullptr;

	GfxImmediatePrimitives* GfxImmediatePrimitives::Get()
	{
		if (Instance == nullptr)
			Instance = new GLImmediatePrimitives();

		return Instance;
	}


	void GfxImmediatePrimitives::Clear()
	{
		SafeDelete(Instance);
	}

	void DrawLine(const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation)
	{
		GfxImmediatePrimitives::Get()->Line(InStart, InEnd, InColor, InTransformation);
	}

	void DrawAxis(const vec3& InPosition, const vec3& Scale, const quat& Orientation)
	{
		GfxImmediatePrimitives::Get()->Axis(InPosition, Scale, Orientation);
	}

	void DrawGrid(const vec3& InPosition, const vec3& Scale)
	{
		GfxImmediatePrimitives::Get()->Grid(InPosition, Scale);
	}
}