#include "GfxImmediatePrimitives.h"
#include "OpenGL/GLImmediatePrimitives.h"
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

	void DrawLine(Camera* InCamera, const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation)
	{
		GfxImmediatePrimitives::Get()->Line(InCamera, InStart, InEnd, InColor, InTransformation);
	}

	void DrawAxis(Camera* InCamera, const vec3& InPosition, const vec3& Scale, const quat& Orientation)
	{
		GfxImmediatePrimitives::Get()->Axis(InCamera, InPosition, Scale, Orientation);
	}

	void DrawGrid(const vec3& InPosition, const vec3& Scale, Camera* InCamera)
	{
		GfxImmediatePrimitives::Get()->Grid(InPosition, Scale, InCamera);
	}
}