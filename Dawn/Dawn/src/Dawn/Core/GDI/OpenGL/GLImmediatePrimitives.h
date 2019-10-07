#pragma once

#include "../GfxImmediatePrimitives.h"
#include "inc_common.h"

namespace Dawn
{
	class GfxGDI;
	class ResourceSystem;

	class GLImmediatePrimitives : public GfxImmediatePrimitives
	{
	public:
		GLImmediatePrimitives(Shared<GfxGDI> GDI);
		void AllocateBuffers(Image* InGridImage, Shader* InPrimitiveShader, Shader* InGridShader) override;
		void Box() override;
		void Line(const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation = mat4(1)) override;
		void Grid(const vec3& InPosition, const vec3& Scale) override;
		void Plane()  override;
		void DrawRay(const Ray& InRay, float InLength, const vec4& InColor, const mat4& model = mat4(1))  override;
		void Axis(const vec3& InPosition, const vec3& Scale = vec3(1), const quat& Orientation = quat()) override;
	};

}