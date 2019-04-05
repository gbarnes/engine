#include "World.h"
#include <iostream>
#include <utility>
#include "Camera/Camera.h"
#include "Transform/Transform.h"
#include "EntityTable.h"

namespace Dawn
{
	World::World()
	{
		Locator::Add(WorldLocatorId, this);
	}

	World::~World()
	{
		Locator::Remove(WorldLocatorId);
	}

	//
	// This function creates a camera entity that is then moved into 
	// a list to save the entity id.
	//
	Camera* World::CreateCamera(std::string& InName, vec3 InPosition, float InAspectRatio,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor)
	{
		EntityId e = EntityTable::Create(InName);
		if (!e.IsValid)
			return nullptr;

		Transform t = MAKE_TRANSFORM(InPosition);
		AddComponent(e, t);

		Camera c = {};
		c.AspectRatio = InAspectRatio;
		c.NearZ = InNearZ;
		c.FarZ = InFarZ;
		c.FieldOfView = InFoV;
		c.ClearColor = InClearColor;
		c.WorldUp = vec3(0, 1, 0);

		auto componentId = AddComponent(e, c);
		if (!componentId.IsValid)
			return nullptr;

		CameraEntities.emplace_back(e);
		return GetComponentById<Camera>(componentId);
	}

	//
	// Returns the camera by a given index (0-last camera)
	//
	Camera* World::GetCamera(u32 InId)
	{
		EntityId e = CameraEntities[InId];
		if (!e.IsValid)
			return nullptr;

		return this->GetComponentByEntity<Camera>(e);
	}
}