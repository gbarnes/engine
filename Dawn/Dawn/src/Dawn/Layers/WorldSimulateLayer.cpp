#include "WorldSimulateLayer.h"
#include "EntitySystem/RigidBody/RigidbodySystem.h"
#include "Application.h"

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
		auto scene = Application->GetPhysics()->GetScene();
		auto world = Application->GetWorld();

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