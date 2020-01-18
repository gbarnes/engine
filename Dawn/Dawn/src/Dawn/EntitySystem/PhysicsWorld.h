#pragma once
/*#pragma comment(lib, "PhysX_64.lib")
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
#pragma comment(lib, "LowLevel_static_64.lib")*/
#include "PxPhysicsAPI.h"

using namespace physx;

#define PX_VISUAL_DEBUGGER_HOST "DawnEnginePX"

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
		static PxDefaultErrorCallback gDefaultErrorCallback;
		static PxDefaultAllocator gDefaultAllocatorCallback;

		bool UseVisualDebugger = false;
		bool RecordMemoryAllocations = false;

		PxCpuDispatcher* CpuDispatcher;
		PxPvd* VisualDebugger;
		PxFoundation* Foundation;
		PxPhysics* Physics;
		PxScene* Scene;
	};
}