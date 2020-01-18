#include "stdafx.h"
#include "WorldSimulateLayer.h"
#include "EntitySystem/RigidBody/RigidbodySystem.h"
#include "Application.h"
#include "EntitySystem/PhysicsWorld.h"

namespace Dawn
{
	WorldSimulateLayer::WorldSimulateLayer(Shared<Dawn::Application> InApplication)
		: Layer(InApplication)
	{
	}

	WorldSimulateLayer::~WorldSimulateLayer()
	{
	}

	void WorldSimulateLayer::Setup()
	{
	}

	void WorldSimulateLayer::FixedUpdate(float InFixedDeltaTime)
	{
		auto scene = Parent->GetPhysics()->GetScene();
		auto world = Parent->GetWorld();

		if (scene) 
		{
			scene->simulate(InFixedDeltaTime);
			scene->fetchResults(true);
		}
	}

	void WorldSimulateLayer::Free()
	{ 	
	}
}