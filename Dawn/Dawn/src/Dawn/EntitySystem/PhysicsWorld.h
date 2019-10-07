#pragma once
#include "inc_core.h"

#define PX_VISUAL_DEBUGGER_HOST "DawnEnginePX"


using namespace physx;
#define PhysicsWorldId std::string("Physics")

namespace Dawn
{
	class DAWN_API PhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

	public:
		bool Initialize();
		void Shutdown();

		inline PxScene* GetScene() const 
		{
			return this->Scene;
		}

	private:
		static physx::PxDefaultErrorCallback gDefaultErrorCallback;
		static physx::PxDefaultAllocator gDefaultAllocatorCallback;

		bool UseVisualDebugger = false;
		bool RecordMemoryAllocations = false;

		PxCpuDispatcher* CpuDispatcher;
		PxPvd* VisualDebugger;
		PxFoundation* Foundation;
		PxPhysics* Physics;
		PxScene* Scene;
	};
}