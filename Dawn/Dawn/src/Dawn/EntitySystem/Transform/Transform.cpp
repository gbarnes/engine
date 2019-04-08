#include "Transform.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(Transform)
	ADD_MEMBER(Transform, Position, vec3)
	ADD_MEMBER(Transform, Scale, vec3)
	ADD_MEMBER(Transform, Rotation, quat)
	MAKE_TYPE_END()
}