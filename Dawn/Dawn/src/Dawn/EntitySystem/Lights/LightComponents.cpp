#include "stdafx.h"
#include "LightComponents.h"
#include "../World.h"
#include "../Transform/Transform.h"
#include "EntitySystem/Camera/Camera.h"
#include "glm.hpp"

namespace Dawn
{
	MAKE_TYPE_BEGIN(DirectionalLight)
		ADD_MEMBER(DirectionalLight, Color, ColorType, true)
		ADD_MEMBER(DirectionalLight, Intensity, FloatType, true)
		ADD_MEMBER(DirectionalLight, bCastShadows, BoolType, true)
	MAKE_TYPE_END()

	MAKE_TYPE_BEGIN(PointLight)
		ADD_MEMBER(PointLight, Color, ColorType, true)
		ADD_MEMBER(PointLight, Intensity, FloatType, true)
		ADD_MEMBER(PointLight, Range, FloatType, true)
		ADD_MEMBER(PointLight, bCastShadows, BoolType, true)
	MAKE_TYPE_END()

	DirectionalLight* LightUtils::CreateDirectionalLight(World* InWorld, quat InOrientation, vec4 InColor, float InIntensity)
	{
		Entity e = InWorld->CreateEntity("Light");
		if (!e.IsValid())
			return nullptr;

		InWorld->CreateTransform(e, vec3(0, 0, 0), vec3(1), InOrientation);

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

		InWorld->CreateTransform(e, InPosition, vec3(1), quat(1, 0, 0, 0));

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
		auto Camera = World::GetActiveCamera();
		auto CamTransform = Camera->GetTransform(Camera->WorldRef);
		auto Transform = InLight->GetEntity().GetTransform(InWorld);

		glm::mat4 Projection = glm::ortho<float>(-65, 65, -65, 65, -65, 65);
		
		glm::mat4 lightView = glm::lookAt
		(
			Transform->Position,
			Transform->Position + (Transform->Forward * 65.0f),
			Transform->Up
		);

		InLight->LightSpace =  Projection * lightView;
	}
}