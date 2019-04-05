#pragma once
#include <inc_common.h>
#include "../Component.h"

namespace Dawn
{
	struct DAWN_API Transform : public Component<Transform>
	{
		static const u32 Version = 1;
		vec3 Position = vec3(0);
		vec3 Scale = vec3(1);
		quat Rotation = quat(0, 0, 0, 1);
	};

#define MAKE_TRANSFORM_PSR(pos, scale, rot) {ComponentId(), pos, scale,  rot}
#define MAKE_TRANSFORM_PS(pos, scale) {ComponentId(), pos, scale,  quat(1, 0, 0, 0)}
#define MAKE_TRANSFORM(pos) {ComponentId(), pos, vec3(1,1,1),  quat(1, 0, 0, 0)}

	class TransformUtils
	{
	public:
		//
		// Calculates the up direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateUp(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return vec3(0.0f);

			return glm::normalize(InTransform->Rotation * vec3(0, 1, 0));
		}

		//
		// Calculates the forward direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateForward(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return vec3(0.0f);
			return vec3(1,1,-1) * glm::normalize(InTransform->Rotation * vec3(0, 0, 1));
		}

		//
		// Calculates the right direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateRight(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return vec3(0.0f);
			return vec3(-1, 1, 1) *  glm::normalize(InTransform->Rotation * vec3(1, 0, 0));
		}


	};

}