#pragma once

#include "../GfxImmediatePrimitives.h"

namespace Dawn
{

	class GLImmediatePrimitives : public GfxImmediatePrimitives
	{
	public:
		GLImmediatePrimitives()
		{
			Instance = this;
		}

		void AllocateBuffers() override;
		void Box() override;
		void Line(Camera* InCamera, const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation = mat4(1)) override;
		void Grid(const vec3& InPosition, const vec3& Scale, Camera* InCamera) override;
		void Plane()  override;
		void DrawRay(Camera* InCamera, const Ray& InRay, float InLength, const vec4& InColor, const mat4& model = mat4(1))  override;
		void Axis(Camera* InCamera, const vec3& InPosition, const vec3& Scale = vec3(1), const quat& Orientation = quat()) override;
	};

}