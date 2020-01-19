#pragma once
#include "EntitySystem/System.h"
#include "Core/Type.h"

namespace Dawn
{
	class ModelViewSystem : public ISystem
	{
		REGISTER_TYPE(ModelViewSystem)
	public:
		virtual DawnType* AccessType() const override
		{
			return ModelViewSystem::GetType();
		}

		virtual void Update(World* InWorld) override;
	};
}