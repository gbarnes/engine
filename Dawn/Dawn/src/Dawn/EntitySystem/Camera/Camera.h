#pragma once
#include "inc_common.h"
#include "../Component.h"
#include "../Transform/Transform.h"
#include "../World.h"


namespace Dawn
{
	class CameraSystem;
	class CameraUtils;

	// Let this only be a normal struct containing plain data
	// without any methods and add the methods to 
	// the camera system and make it the main access point for cameras ?
	// 
	struct DAWN_API Camera : public Component<Camera>
	{
		friend class CameraUtils;
	public:
		static const u32 Version = 1;
		float FieldOfView = 65.0f;
		float NearZ = 0.1f;
		float FarZ = 100.0f;
		float AspectRatio = 0;

		vec4 ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };
		vec3 WorldUp = {0.0f,1.0f,0.0f};

		Transform* GetTransform()
		{
			auto world = World::Get();
			return world->GetComponentByEntity<Transform>(this->Id.Entity);
		}

		const mat4& GetProjection() const 
		{
			return Projection;
		}

		const mat4& GetView() const
		{
			return View;
		}

	private:
		mat4 Projection;
		mat4 View;
	};

	//
	// Helper function to get a camera from the world!
	//
	static Camera* GetCamera(u32 i)
	{
		auto world = World::Get();
		return world->GetCamera(i);
	};

	//
	// Helper function to create a new camera at the world!
	//
	static Camera* CreateCamera(std::string&& InName, vec3 InPosition, float InAspectRatio,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor)
	{
		auto world = World::Get();
		return world->CreateCamera(InName, InPosition, InAspectRatio, InNearZ, InFarZ, InFoV, InClearColor);
	};

	class CameraUtils
	{
	public:
		//
		// Takes care of casting the quaternion rotation to a rotation matrix
		//
		static mat4 CalculateOrientation(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return mat4(1.0f);

			return glm::mat4_cast(InTransform->Rotation);
		}

		//
		// Calculates the view matrix from the given orientation and translation matrix
		//
		static const mat4& CalculateView(Camera* InCamera, Transform* InTransform)
		{
			InCamera->View = CalculateOrientation(InTransform) * glm::translate(glm::mat4(1.0f), -InTransform->Position);
			return InCamera->View;
		}

		//
		// Calculates the perspective matrix for the given camera.
		//
		static const mat4& CalculatePerspective(Camera* InCamera)
		{
			InCamera->Projection = glm::perspective(glm::radians(InCamera->FieldOfView), InCamera->AspectRatio, InCamera->NearZ, InCamera->FarZ);
			return InCamera->Projection;
		}


		const vec3 WorldToScreenPoint(Camera* InCamera, const vec3& InPoint) const
		{
			//return glm::project(InPoint, mat4(1.0f), GetProjection(), InPoint);
			return { 0,0,0 };
		}

		const vec3 ScreenToWorldPoint(Camera* InCamera, const vec3& InPoint) const
		{
			//return DirectX::XMVector3Unproject(InPoint, 0, 0, 0, 0, 0, 1, Projection, CalculateView(), MAT4_IDENTITY);
			//return glm::unProject(InPoint, InCamera->View, InCamera->Projection, );
			return { 0,0,0 };
		}

	};
}