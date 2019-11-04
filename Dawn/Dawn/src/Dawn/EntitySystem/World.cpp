#include "World.h"
#include <iostream>
#include <utility>
#include "Camera/Camera.h"
#include "Transform/Transform.h"
#include "Lights/LightComponents.h"
#include "EntityTable.h"

namespace Dawn
{
	World::World()
	{
	}

	World::~World()
	{
	}

	void World::Shutdown()
	{
	}

	//
	// This function creates a camera entity that is then moved into 
	// a list to save the entity id.
	//
	Camera* World::CreateCamera(std::string& InName, u32 Width, u32 Height,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor, 
		const vec3& InPosition, const quat& InOrientation)
	{
		Entity e = CreateEntity(InName);
		if (!e.IsValid())
			return nullptr;

		Transform* t = this->MakeComponent<Transform>();
		t->Position = InPosition;
		t->Rotation = InOrientation;
		t->Scale = vec3(1, 1, 1);
		AddComponent<Transform>(e, t);

		Camera* c = this->MakeComponent<Camera>();
		c->AspectRatio = (float)Width / (float)Height;
		c->Height = Height;
		c->Width = Width;
		c->NearZ = InNearZ;
		c->FarZ = InFarZ;
		c->FieldOfView = InFoV;
		c->ClearColor = InClearColor;
		c->WorldUp = vec3(0, 1, 0);

		auto componentId = AddComponent<Camera>(e, c);
		if (!componentId.IsValid)
			return nullptr;

		CameraEntities.emplace_back(e.Id);
		return c;
	}

	//
	// Returns the camera by a given index (0-last camera)
	//
	Camera* World::GetCamera(const i32 InId)
	{
		i32 e = CameraEntities[InId];
		Entity entity = EntityTable::Get(e);

		if (!entity.IsValid())
			return nullptr;

		return this->GetComponentByEntity<Camera>(entity);
	}

	std::vector<Camera*> World::GetCameras()
	{ 
		std::vector<Camera*> Cams;

		for (auto CamId : CameraEntities)
		{
			Entity entity = EntityTable::Get(CamId);
			auto CamRef = GetComponentByEntity<Camera>(entity);
			if (CamRef)
				Cams.push_back(CamRef);
		}

		return Cams;
	}

	//
	// Creates a new Entity with the given name and returns it's 
	// entity handle.
	//
	inline Entity World::CreateEntity(const std::string &InName) const
	{
		return EntityTable::Create(InName);
	}

	//
	// Returns all the component type names for a specific 
	// entity! This is mainly used for editor handling.
	//
	std::vector<std::string> World::GetComponentTypesByEntity(const Entity& InEntity)
	{
		std::vector<std::string> components;

		for (int i = 0; i < ComponentTables.size(); ++i)
		{
			// once we hit the first empty table pointer
			// we just break since there won't be anymore tables ahead.
			if (ComponentTables[i] == nullptr)
				break;

			if (ComponentTables[i]->ComponentForEntityExists(InEntity))
				components.emplace_back(ComponentTables[i]->GetTypeName());
		}

		return components;
	}

}