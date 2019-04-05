#pragma once

#include "inc_common.h"

namespace Dawn
{
	enum DbgPrimitiveType
	{
		Quad,
		Line,
		Box, 
		Plane
	};

	struct Ray
	{
		vec3 Pos;
		vec3 Dir;
	};

	struct Camera;

	namespace Debug 
	{
		void AllocateResources();
		void Box();
		void Line();
		void Quad(vec3& InPosition, Camera* InCamera);
		void Plane();
		void DrawRay(Camera* InCamera, const Ray& InRay, float InLength, const vec4& InColor);
	}
}