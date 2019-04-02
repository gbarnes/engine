#pragma once
#include "inc_common.h"
#include "../Component.h"

namespace Dawn
{
	class CameraSystem;

	// Let this only be a normal struct containing plain data
	// without any methods and add the methods to 
	// the camera system and make it the main access point for cameras ?
	// 

	struct DAWN_API Camera : public Component<Camera>
	{
	public:
		static const u32 Version = 1;
		float FieldOfView = 65.0f;
		float NearZ = 0.1f;
		float FarZ = 100.0f;
		float AspectRatio = 0;

		vec4 ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };

		vec3 Position = { 0.0f, 0.0f, 0.0f };
		quat Rotation = { 1,0,0,0 };
		vec3 Up = { 0.0f, 1.0f, 0.0f };
		vec3 Target = {0.0f, 0.0f, 0.0f};

		const mat4& CalculateView()
		{
			View = glm::lookAt(Position, Target, Up);
			return View;
		}

		const mat4& CalculatePerspective()
		{
			Projection = glm::perspective(glm::radians(FieldOfView), AspectRatio, NearZ, FarZ);
			return Projection;
		}

		const vec3 CalculateUp()
		{
			Up = glm::cross(GetDirection(), GetRight());
			return Up;
		}


		const vec3 GetDirection() const
		{
			return glm::normalize(Position - Target);
		}

		const vec3 GetRight() const
		{
			return glm::normalize(glm::cross(Up, GetDirection()));
		}

		const mat4& GetProjection() const 
		{
			return Projection;
		}

		const mat4& GetView() const
		{
			return View;
		}

		const vec3 WorldToScreenPoint(const vec3& InPoint) const
		{
			//return glm::project(InPoint, mat4(1.0f), Projection, InPoint);
			return { 0,0,0 };
		}

		const vec3 ScreenToWorldPoint(const vec3& InPoint) const
		{
			//return DirectX::XMVector3Unproject(InPoint, 0, 0, 0, 0, 0, 1, Projection, CalculateView(), MAT4_IDENTITY);
			return { 0,0,0 };
		}

	private:
		mat4 Projection;
		mat4 View;
	};
}