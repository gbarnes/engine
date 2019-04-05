#include "Entity.h"
#include "World.h"
#include "Transform/Transform.h"

namespace Dawn
{
	Transform* Entity::GetTransform()
	{
		auto world = World::Get();
		return world->GetComponentByEntity<Transform>(Id);
	}
}