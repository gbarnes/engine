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
		REGISTER_TYPE(Camera)
		static const u32 Version = 1;
		float FieldOfView = 65.0f;
		float NearZ = 0.1f;
		float FarZ = 100.0f;
		float AspectRatio = 0;
		u32 Width = 800;
		u32 Height = 600;
		bool bIsOrthographic = false;

		vec4 ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };
		vec3 WorldUp = {0.0f,1.0f,0.0f};

		Transform* GetTransform(World* InWorld)
		{
			return InWorld->GetComponentByEntity<Transform>(this->Id.Entity);
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
		mat4 Projection = mat4(1);
		mat4 View = mat4(1);
	};

	//
	// Helper function to get a camera from the world!
	//
	static Camera* GetCamera(World* InWorld, u32 i)
	{
		return InWorld->GetCamera(i);
	};

	//
	// Helper function to create a new camera at the world!
	//
	static Camera* CreateCamera(World* InWorld, std::string&& InName, u32 Width, u32 Height,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor, 
		const vec3& InPosition = vec3(0), const quat& InOrientation = quat())
	{
		return InWorld->CreateCamera(InName, Width, Height, InNearZ, InFarZ, InFoV, InClearColor, InPosition, InOrientation);
	};

	class CameraUtils
	{
	public:
		

		//
		// Calculates the view matrix from the given orientation and translation matrix
		//
		FORCEINLINE static const mat4& CalculateView(Camera* InCamera, Transform* InTransform)
		{
			InCamera->View = glm::mat4_cast(glm::conjugate(InTransform->Rotation));
			InCamera->View = glm::translate(InCamera->View, -InTransform->Position);
			return InCamera->View;
		}

		FORCEINLINE static const mat4& CalculateView2(Camera* InCamera, Transform* InTransform)
		{
			//InCamera->View = glm::mat4_cast(InTransform->Rotation);
			InCamera->View = glm::lookAt(InTransform->Position, vec3(0, 0, -1), vec3(0,1,0)); //glm::translate(InCamera->View, InTransform->Position);
			return InCamera->View;
		}


		//
		// Calculates the perspective matrix for the given camera.
		//
		FORCEINLINE static const mat4& CalculatePerspective(Camera* InCamera)
		{
			InCamera->Projection = glm::perspective(glm::radians(InCamera->FieldOfView), InCamera->AspectRatio, InCamera->NearZ, InCamera->FarZ);
			return InCamera->Projection;
		}

		//
		// Calculates the orthographic matrix for the given camera.
		//
		FORCEINLINE static const mat4& CalculateOthographic(Camera* InCamera)
		{
			InCamera->Projection = glm::ortho<float>(0.0f, (float)InCamera->Width, 0.0f, (float)InCamera->Height, InCamera->NearZ, InCamera->FarZ);
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