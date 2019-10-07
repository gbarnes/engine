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

	void WorldSimulateLayer::Process()
	{
		auto scene = Application->GetPhysics()->GetScene();
		auto world = Application->GetWorld();
		auto time = Timer::GetTime();

		if (scene) 
		{
			scene->simulate(time.GetDeltaSeconds());
			scene->fetchResults(true);
		}
		
		// process rigidbodys
		auto system = world->GetSystemByType<RigidbodySystem>(RigidbodySystem::GetType());
		if(system)
			system->Update(world.get());
	}

	void WorldSimulateLayer::Free()
	{ 	
	}
}