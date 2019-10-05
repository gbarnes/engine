#include "LightComponents.h"
#include "../World.h"
#include "../Transform/Transform.h"

namespace Dawn
{
	DirectionalLight* LightUtils::CreateDirectionalLight(vec3 InPosition, quat InOrientation, vec4 InColor)
	{
		auto World = GetWorld();

		EntityId e = World->CreateEntity("Light");
		if (!e.IsValid)
			return nullptr;

		Transform* t = World->MakeComponent<Transform>();
		t->Position = InPosition;
		t->Rotation = InOrientation;
		t->Scale = vec3(1, 1, 1);
		World->AddComponent<Transform>(e, t);

		DirectionalLight* l = World->MakeComponent<DirectionalLight>();
		l->Color = InColor;
		World->AddComponent<DirectionalLight>(e, l);
		return l;
	}

	PointLight* LightUtils::CreatePointLight(vec3 InPosition, vec4 InColor, float InRange)
	{
		return nullptr;
	}

	SpotLight* LightUtils::CreateSpotLight(vec3 InPosition, quat InOrientation, vec4 InColor, float InAngle, float InRange)
	{
		return nullptr;
	}
}