#pragma once

#include "inc_common.h"
#include "inc_core.h"
#include "Transform/Transform.h"

namespace Dawn
{
	class World;

	class ISystem
	{
	public:
		~ISystem() = default;

		virtual DawnType* AccessType() const = 0;
		virtual void Update(World* InWorld) = 0;
	};

	/*class MovementSystem : public ISystem
	{
		REGISTER_TYPE(MovementSystem)
	public:

		Type* AccessType() const override
		{
			return MovementSystem::GetType();
		}

		// Geerbt über ISystem
		virtual void Update(Dawn::World * InWorld) override;
	};*/

}