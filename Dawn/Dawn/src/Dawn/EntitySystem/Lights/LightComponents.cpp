#include "LightComponents.h"
#include "../World.h"
#include "../Transform/Transform.h"
#include "glm.hpp"

namespace Dawn
{
	MAKE_TYPE_BEGIN(DirectionalLight)
		ADD_MEMBER(DirectionalLight, Color, vec4)
		ADD_MEMBER(DirectionalLight, Intensity, float)
		ADD_MEMBER(DirectionalLight, bCastShadows, bool)
	MAKE_TYPE_END()

	MAKE_TYPE_BEGIN(PointLight)
		ADD_MEMBER(PointLight, Color, vec4)
		ADD_MEMBER(PointLight, Intensity, float)
		ADD_MEMBER(PointLight, Range, float)
		ADD_MEMBER(PointLight, bCastShadows, bool)
	MAKE_TYPE_END()

	DirectionalLight* LightUtils::CreateDirectionalLight(World* InWorld, quat InOrientation, vec4 InColor, float InIntensity)
	{
		Entity e = InWorld->CreateEntity("Light");
		if (!e.IsValid())
			return nullptr;

		Transform* t = InWorld->MakeComponent<Transform>();
		t->Position = vec3(0, 0, 0);
		t->Rotation = InOrientation;
		t->Scale = vec3(1, 1, 1);
		InWorld->AddComponent<Transform>(e, t);

		DirectionalLight* l = InWorld->MakeComponent<DirectionalLight>();
		l->Color = InColor;
		l->Intensity = InIntensity;
		InWorld->AddComponent<DirectionalLight>(e, l);
		return l;
	}

	PointLight* LightUtils::CreatePointLight(World* InWorld, vec3 InPosition, vec4 InColor, float InIntensity, float InRange)
	{
		Entity e = InWorld->CreateEntity("Pointlight");
		if (!e.IsValid())
			return nullptr;

		Transform* t = InWorld->MakeComponent<Transform>();
		t->Position = InPosition;
		t->Rotation = quat(1,0,0,0);
		t->Scale = vec3(1, 1, 1);
		InWorld->AddComponent<Transform>(e, t);

		PointLight* l = InWorld->MakeComponent<PointLight>();
		l->Color = InColor;
		l->Range = InRange;
		l->Intensity = InIntensity;
		InWorld->AddComponent<PointLight>(e, l);
		return l;
	}

	SpotLight* LightUtils::CreateSpotLight(vec3 InPosition, quat InOrientation, vec4 InColor, float InAngle, float InRange)
	{
		return nullptr;
	}

	void LightUtils::CalculateOrthoLightMatrix(World* InWorld, DirectionalLight* InLight, float InNearPlane, float InFarPlane)
	{
		auto Transform = InLight->GetEntity().GetTransform(InWorld);

		glm::mat4 Projection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, InNearPlane, InFarPlane);
		glm::mat4 lightView = glm::lookAt
		(
			Transform->Position - Transform->Forward * 20.0f,
			Transform->Position + Transform->Forward * -2.0f,
			Transform->Up
		);

		InLight->LightSpace = Projection * lightView;
	}
}