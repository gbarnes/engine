#pragma once
#include "../System.h"
#include "../World.h"
#include "../Transform/Transform.h"
#include "../Camera/Camera.h"

namespace Dawn
{
	class RigidbodySystem : public ISystem
	{
		REGISTER_TYPE(RigidbodySystem)

	public:
		virtual Type* AccessType() const override
		{
			return RigidbodySystem::GetType();
		}

		virtual void Update(World* InWorld) override
		{
			auto components = InWorld->GetComponentSets<Camera, Transform>();
			for (std::pair<Camera*, Transform*> pair : components)
			{
				//DWN_CORE_INFO("Components: {0}, {1}!", pair.first->Id.Index, pair.second->Id.Index);
			}
		}
	};

}