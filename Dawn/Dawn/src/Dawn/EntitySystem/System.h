#pragma once

#include "inc_common.h"
#include "inc_core.h"
#include "Transform/Transform.h"
#include "World.h"

namespace Dawn
{
	template<typename T>
	class System
	{
	public:
		virtual void Update(World* InWorld) = 0;
	};

	class Transform;
	class MovementSystem : System<MovementSystem>
	{
		void Update(World* InWorld) override
		{
			auto transforms = InWorld->GetComponentsByType<Transform>();
			for (auto transform : transforms)
			{
				//auto modelmatrix = Mat4() * transform->Position * transform->Rotation * transform->Scale;
			}
		}
	};
}