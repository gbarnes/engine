#pragma once
#include <inc_common.h>
#include <inc_core.h>
#include "../SceneGraph.h"
#include "../Component.h"

namespace Dawn
{
	struct DAWN_API Transform : public Component<Transform>
	{
		REGISTER_TYPE(Transform)
		static const u32 Version = 1;

		Transform()
		{}

		Transform(vec3 InPos, vec3 InScale, quat InRotation)
			: Position(InPos), Scale(InScale), Rotation(InRotation)
		{}

		vec3 Position = vec3(0);
		vec3 Scale = vec3(1);
		quat Rotation = quat(0, 0, 0, 1);

	private:
		SceneNodeId SceneId;

		friend class SceneGraph;
	};

#define MAKE_TRANSFORM_PSR(pos, scale, rot) Transform(pos, scale,  rot)
#define MAKE_TRANSFORM_PS(pos, scale) Transform(pos, scale,  quat(1, 0, 0, 0))
#define MAKE_TRANSFORM(pos) Transform(pos, vec3(1,1,1), quat(1, 0, 0, 0))

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