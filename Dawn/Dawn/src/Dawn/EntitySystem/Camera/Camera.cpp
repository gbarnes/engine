#include "stdafx.h"
#include "Camera.h"
#include "../Transform/Transform.h"
#include "../World.h"
#include "Application.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(Camera)
		ADD_MEMBER(Camera, FieldOfView, FloatType, true)
		ADD_MEMBER(Camera, NearZ, FloatType, true)
		ADD_MEMBER(Camera, FarZ, FloatType, true)
		ADD_MEMBER(Camera, AspectRatio, FloatType, true)
		ADD_MEMBER(Camera, ClearColor, ColorType, true)
		ADD_MEMBER(Camera, WorldUp, Vector3Type, true)
		ADD_MEMBER(Camera, bIsOrthographic, BoolType, true)
	MAKE_TYPE_END()

	Transform* Camera::GetTransform(World* InWorld)
	{
		D_ASSERT(InWorld != nullptr, "World cannot be null!");
		return InWorld->GetComponentByEntity<Transform>(this->Id.Entity);
	}

	const mat4& Camera::GetProjection() const
	{
		return Projection;
	}

	const mat4& Camera::GetView() const
	{
		return View;
	}

	void Camera::InitFromLoad(World* World, void* Component)
	{
		Camera* cam = static_cast<Camera*>(Component);
		D_ASSERT(cam != nullptr, "Camera is null when init");
		World->AddCamera(cam);
		
		cam->WorldRef = World;
		cam->Width = g_Application->GetSettings()->Width;
		cam->Height = g_Application->GetSettings()->Height;

		if (cam->bIsOrthographic)
			CameraUtils::CalculateOthographic(cam);
		else
			CameraUtils::CalculatePerspective(cam);

		CameraUtils::CalculateView(cam, cam->GetTransform(World));
	}

	Camera* GetCamera(World* InWorld, u32 i)
	{
		return InWorld->GetCamera(i);
	};

	Camera* CreateCamera(World* InWorld, std::string&& InName, u32 Width, u32 Height,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor,
		const vec3& InPosition, const quat& InOrientation)
	{
		return InWorld->CreateCamera(InName, Width, Height, InNearZ, InFarZ, InFoV, InClearColor, InPosition, InOrientation);
	};



	const mat4& CameraUtils::CalculateView(Camera* InCamera, Transform* InTransform)
	{
		InCamera->View = glm::mat4_cast(glm::conjugate(InTransform->Rotation));
		InCamera->View = glm::translate(InCamera->View, -InTransform->Position);
		return InCamera->View;
	}

	const void CameraUtils::CameraUtils::CalculateFrustum(Camera* InCamera)
	{

	}

	const mat4& CameraUtils::CameraUtils::CalculatePerspective(Camera* InCamera)
	{
		InCamera->Projection = glm::perspective(glm::radians(InCamera->FieldOfView), InCamera->AspectRatio, InCamera->NearZ, InCamera->FarZ);
		return InCamera->Projection;
	}

	const mat4& CameraUtils::CalculateOthographic(Camera* InCamera)
	{
		InCamera->Projection = glm::ortho<float>(0.0f, (float)InCamera->Width, -(float)InCamera->Height, (float)InCamera->Height, InCamera->NearZ, InCamera->FarZ);
		return InCamera->Projection;
	}

	vec2 CameraUtils::WorldToScreenPoint(Camera* InCamera, const vec3& InPoint)
	{
		vec2 Pos = glm::project(InPoint, InCamera->GetView(), InCamera->GetProjection(), glm::vec4(0.0f, float(InCamera->Height), float(InCamera->Width), -float(InCamera->Height)));
		return vec2(Pos.x, Pos.y);
	}

	vec3 CameraUtils::ScreenToWorldPoint(Camera* InCamera, const vec3& InPoint)
	{
		return glm::unProject(InPoint, InCamera->GetView(), InCamera->GetProjection(), glm::vec4(0.0f, 0.0f, float(InCamera->Width), float(InCamera->Height)));
	}
}