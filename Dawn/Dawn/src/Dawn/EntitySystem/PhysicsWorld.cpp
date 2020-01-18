#include "stdafx.h"

#include "PhysicsWorld.h"
#include "Core/Logging/Log.h"

using namespace physx;

namespace Dawn
{
	PxDefaultErrorCallback PhysicsWorld::gDefaultErrorCallback;
	PxDefaultAllocator PhysicsWorld::gDefaultAllocatorCallback;

	PhysicsWorld::PhysicsWorld()
	{
	} 

	PhysicsWorld::~PhysicsWorld()
	{
	}

	bool PhysicsWorld::Initialize()
	{
		Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (!Foundation)
		{
			DWN_CORE_ERROR("Couldn't initialize physx foundation with version {0} !", PX_PHYSICS_VERSION);
			return false;
		}

		VisualDebugger = nullptr;
		if (UseVisualDebugger)
		{
			VisualDebugger = PxCreatePvd(*Foundation);
			if (VisualDebugger)
			{
				PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PX_VISUAL_DEBUGGER_HOST, 5425, 10);
				VisualDebugger->connect(*transport, PxPvdInstrumentationFlag::eALL);
			}
		}

		Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), RecordMemoryAllocations, VisualDebugger);
		if (!Physics)
		{
			DWN_CORE_ERROR("Couldn't create physics class!");
			Foundation->release();
			return false;
		}

		PxSceneDesc sceneDesc = PxSceneDesc(Physics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0, -9.81f, 0);
		sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		if (!sceneDesc.cpuDispatcher)
		{
			CpuDispatcher = PxDefaultCpuDispatcherCreate(1);
			if (!CpuDispatcher)
				DWN_CORE_ERROR("PxDefaultCpuDispatcherCreate failed!");

			sceneDesc.cpuDispatcher = CpuDispatcher;
		}

		sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
		sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
		sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;
		sceneDesc.sceneQueryUpdateMode = PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
		sceneDesc.gpuMaxNumPartitions = 8;

		this->Scene = Physics->createScene(sceneDesc);
		if (!this->Scene)
		{
			DWN_CORE_ERROR("Couldn't create physics class!");

			Physics->release();
			Foundation->release();
			
			return false;
		}

		this->Scene->setGravity(PxVec3(0.0f, -9.97f, 0.0f));

		return true;
	}

	void PhysicsWorld::Shutdown()
	{
		if (Scene)
			Scene->release();

		if (Physics)
			Physics->release();

		if (Foundation)
			Foundation->release();

	}

}