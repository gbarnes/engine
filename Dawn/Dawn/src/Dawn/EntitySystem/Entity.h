#pragma once
#include "inc_common.h"

namespace Dawn
{
	typedef GenericHandle EntityId;
	struct Transform;
	class World;

	struct DAWN_API Entity
	{
		EntityId Id;
		std::string Name;
		bool PersistsThroughLoad = false;

		Transform* GetTransform(World* InWorld);
	};
}