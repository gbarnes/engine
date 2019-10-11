#pragma once
#include "inc_core.h"
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "PhysXVehicle_static_64.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_64.lib")
#pragma comment(lib, "LowLevelDynamics_static_64.lib")
#pragma comment(lib, "LowLevelAABB_static_64.lib")
#pragma comment(lib, "FastXml_static_64.lib")
#pragma comment(lib, "LowLevel_static_64.lib")

#include "PxPhysicsAPI.h"

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