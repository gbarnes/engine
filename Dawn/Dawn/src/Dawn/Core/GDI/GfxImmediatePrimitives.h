#pragma once

#include "inc_common.h"

namespace Dawn
{
	struct Ray
	{
		vec3 Pos;
		vec3 Dir;
	};

	struct Camera;

	//
	// Provides immediate drawing for basic primitives.
	//
	class DAWN_API GfxImmediatePrimitives
	{
	public:
		~GfxImmediatePrimitives() = default;

		virtual void AllocateBuffers() = 0;
		virtual void Box() = 0;
		virtual void Line(const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation = mat4(1)) = 0;
		virtual void Grid(const vec3& InPosition, const vec3& Scale) = 0;
		virtual void Plane() = 0;
		virtual void DrawRay(const Ray& InRay, float InLength, const vec4& InColor, const mat4& model = mat4(1)) = 0;
		virtual void Axis(const vec3& InPosition, const vec3& Scale = vec3(1), const quat& Orientation = quat()) = 0;

		void SetCamera(Camera* InCamera)
		{
			this->Camera = InCamera;
		}

		static GfxImmediatePrimitives* Get();
		static void Clear();
	protected:
		GfxImmediatePrimitives()
		{
		}

		Camera* Camera;
		static GfxImmediatePrimitives* Instance;

	};

	void DrawLine(const vec3& InStart, const vec3& InEnd, const vec4& InColor, const mat4& InTransformation);
	void DrawAxis(const vec3& InPosition, const vec3& Scale, const quat& Orientation);
	void DrawGrid(const vec3& InPosition, const vec3& Scale);
}