#include "WorldSimulateLayer.h"
#include "EntitySystem/RigidBody/RigidbodySystem.h"

namespace Dawn
{
	WorldSimulateLayer::WorldSimulateLayer()
	{
	}

	WorldSimulateLayer::~WorldSimulateLayer()
	{
	}

	void WorldSimulateLayer::Setup()
	{
		Physics = PhysicsWorld::Get();
		Physics->AddRef();

		World = World::Get();
		World->AddRef();
	}

	void WorldSimulateLayer::Process()
	{
		auto scene = Physics->GetScene();
		auto time = Timer::GetTime();

		if (scene) 
		{
			scene->simulate(time.GetDeltaSeconds());
			scene->fetchResults(true);
		}

		// process rigidbodys
		auto system = World->GetSystemByType<RigidbodySystem>(RigidbodySystem::GetType());
		if(system)
			system->Update(World.Get());
	}

	void WorldSimulateLayer::Free()
	{
		World->Release();
		Physics->Release(); 	
	}
}