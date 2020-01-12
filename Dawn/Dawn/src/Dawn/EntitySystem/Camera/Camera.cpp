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

	void Camera::InitFromLoad(World* World, void* Component)
	{
		Camera* cam = static_cast<Camera*>(Component);
		D_ASSERT(cam != nullptr, "Camera is null when init");
		World->AddCamera(cam);
		
		cam->WorldRef = World;

		if (cam->bIsOrthographic)
			CameraUtils::CalculateOthographic(cam);
		else
			CameraUtils::CalculatePerspective(cam);

		CameraUtils::CalculateView(cam, cam->GetTransform(World));
	}
}