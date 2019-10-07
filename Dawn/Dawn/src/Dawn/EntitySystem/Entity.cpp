#include "Entity.h"
#include "World.h"
#include "Transform/Transform.h"

namespace Dawn
{
	Transform* Entity::GetTransform(World* InWorld)
	{
		return InWorld->GetComponentByEntity<Transform>(Id);
	}
}