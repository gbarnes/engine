#include "LightComponents.h"
#include "../World.h"
#include "../Transform/Transform.h"

namespace Dawn
{
	//MAKE_TYPE_BEGIN(DirectionalLight)
	//	ADD_MEMBER(DirectionalLight, Color, vec4)
	//MAKE_TYPE_END()

	DirectionalLight* LightUtils::CreateDirectionalLight(World* InWorld, quat InOrientation, vec4 InColor, float InIntensity)
	{
		EntityId e = InWorld->CreateEntity("Light");
		if (!e.IsValid)
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
		EntityId e = InWorld->CreateEntity("Pointlight");
		if (!e.IsValid)
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
}