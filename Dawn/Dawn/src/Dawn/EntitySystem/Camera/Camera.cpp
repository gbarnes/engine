#include "Camera.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(Camera)
		ADD_MEMBER(Camera, FieldOfView, FloatType, true)
		ADD_MEMBER(Camera, NearZ, FloatType, true)
		ADD_MEMBER(Camera, FarZ, FloatType, true)
		ADD_MEMBER(Camera, AspectRatio, FloatType, true)
		ADD_MEMBER(Camera, ClearColor, ColorType, true)
		ADD_MEMBER(Camera, WorldUp, Vector3Type, true)
		ADD_MEMBER(Camera, bIsOrthographic, BoolType, true)
	MAKE_TYPE_END()
}