#include "Camera.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(Camera)
		ADD_MEMBER(Camera, FieldOfView, float)
		ADD_MEMBER(Camera, NearZ, float)
		ADD_MEMBER(Camera, FarZ, float)
		ADD_MEMBER(Camera, AspectRatio, float)
		ADD_MEMBER(Camera, ClearColor, vec4)
		ADD_MEMBER(Camera, WorldUp, vec3)
		ADD_MEMBER(Camera, bIsOrthographic, bool)
	MAKE_TYPE_END()
}