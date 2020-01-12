#include "Transform.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(Transform)
	ADD_MEMBER(Transform, Position, Vector3Type, true)
	ADD_MEMBER(Transform, Scale, Vector3Type, true)
	ADD_MEMBER(Transform, Rotation, QuatType, true)
	MAKE_TYPE_END()

	void Transform::InitFromLoad(World* InWorld, void* Component)
	{
		auto transform = static_cast<Transform*>(Component);
		transform->WorldRef = InWorld;

		TransformUtils::CalculateForward(transform);
		TransformUtils::CalculateUp(transform);
		TransformUtils::CalculateRight(transform);
	}
}