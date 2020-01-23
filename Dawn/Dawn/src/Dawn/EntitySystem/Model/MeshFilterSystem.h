#pragma once
#include "EntitySystem/System.h"
#include "Core/Type.h"

namespace Dawn
{
	class MeshFilterSystem : public ISystem
	{
		REGISTER_TYPE(MeshFilterSystem)
	public:
		virtual DawnType* AccessType() const override
		{
			return MeshFilterSystem::GetType();
		}

		virtual void Update(World* InWorld) override;
	};
}