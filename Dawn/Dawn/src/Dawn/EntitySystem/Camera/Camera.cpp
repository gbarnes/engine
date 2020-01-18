#include "stdafx.h"
#include "Camera.h"
#include "../Transform/Transform.h"
#include "../World.h"
#include "Application.h"

namespace Dawn
{
	Frustum::Frustum(mat4 m)
	{
		m = glm::transpose(m);
		Planes[Left] = m[3] + m[0];
		Planes[Right] = m[3] - m[0];
		Planes[Bottom] = m[3] + m[1];
		Planes[Top] = m[3] - m[1];
		Planes[Near] = m[3] + m[2];
		Planes[Far] = m[3] - m[2];

		vec3 crosses[Combinations] = {
			glm::cross(vec3(Planes[Left]),   vec3(Planes[Right])),
			glm::cross(vec3(Planes[Left]),   vec3(Planes[Bottom])),
			glm::cross(vec3(Planes[Left]),   vec3(Planes[Top])),
			glm::cross(vec3(Planes[Left]),   vec3(Planes[Near])),
			glm::cross(vec3(Planes[Left]),   vec3(Planes[Far])),
			glm::cross(vec3(Planes[Right]),  vec3(Planes[Bottom])),
			glm::cross(vec3(Planes[Right]),  vec3(Planes[Top])),
			glm::cross(vec3(Planes[Right]),  vec3(Planes[Near])),
			glm::cross(vec3(Planes[Right]),  vec3(Planes[Far])),
			glm::cross(vec3(Planes[Bottom]), vec3(Planes[Top])),
			glm::cross(vec3(Planes[Bottom]), vec3(Planes[Near])),
			glm::cross(vec3(Planes[Bottom]), vec3(Planes[Far])),
			glm::cross(vec3(Planes[Top]),    vec3(Planes[Near])),
			glm::cross(vec3(Planes[Top]),    vec3(Planes[Far])),
			glm::cross(vec3(Planes[Near]),   vec3(Planes[Far]))
		};

		Points[0] = intersection<Left, Bottom, Near>(crosses);
		Points[1] = intersection<Left, Top, Near>(crosses);
		Points[2] = intersection<Right, Bottom, Near>(crosses);
		Points[3] = intersection<Right, Top, Near>(crosses);
		Points[4] = intersection<Left, Bottom, Far>(crosses);
		Points[5] = intersection<Left, Top, Far>(crosses);
		Points[6] = intersection<Right, Bottom, Far>(crosses);
		Points[7] = intersection<Right, Top, Far>(crosses);

	}

	


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

		CalculateFrustum(InCamera);

		return InCamera->View;
	}

	const void CameraUtils::CameraUtils::CalculateFrustum(Camera* InCamera)
	{
		InCamera->Frustum = Frustum(InCamera->Projection * InCamera->View);
	}

	const mat4& CameraUtils::CameraUtils::CalculatePerspective(Camera* InCamera)
	{
		InCamera->Projection = glm::perspective(glm::radians(InCamera->FieldOfView), InCamera->AspectRatio, InCamera->NearZ, InCamera->FarZ);
		CalculateFrustum(InCamera);
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

	
	bool CameraUtils::IsBoxVisible(const Frustum& InFrustum, const vec3& minp, const vec3& maxp)
	{
		// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm

		// check box outside/inside of frustum
		for (int i = 0; i < Frustum::Planes::Count; i++)
		{
			if ((glm::dot(InFrustum.Planes[i], vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
				(glm::dot(InFrustum.Planes[i], vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
			{
				return false;
			}
		}

		// check frustum outside/inside box
		int out;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; i++) out += ((InFrustum.Points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

		return true;
	}

	bool CameraUtils::IsPointVisible(const Frustum& InFrustum, const vec3& InPoint)
	{
		for (int i = 0; i < Frustum::Planes::Count; i++)
		{
			if (glm::dot(InFrustum.Planes[i], vec4(InPoint.x, InPoint.y, InPoint.z, 1.0f)) < 0.0)
				return false;
		}

		return true;
	}

}