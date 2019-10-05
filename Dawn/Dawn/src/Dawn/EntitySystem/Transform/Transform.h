#pragma once
#include "inc_common.h"
#include "inc_core.h"
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
		quat Rotation = quat();

		vec3 Forward = vec3(0, 0, 1);
		vec3 Up = vec3(0, 1, 0);
		vec3 Right = vec3(1, 0, 0);

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
		// Takes care of casting the quaternion rotation to a rotation matrix
		//
		static mat4 GetRotationMatrix(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return mat4(1.0f);

			return glm::mat4_cast(InTransform->Rotation);
		}

		//
		// 
		//
		static void Rotate(Transform* InTransform, const quat& Rotation)
		{
			InTransform->Rotation = Rotation;
		}

		//
		// Calculates the up direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateUp(Transform* InTransform)
		{
			if (InTransform == nullptr) 
				return vec3(0.0f);

			InTransform->Up = InTransform->Rotation * glm::vec3(0, 1, 0);
			return InTransform->Up;
		}

		//
		// Calculates the forward direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateForward(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return vec3(0.0f);

			InTransform->Forward = InTransform->Rotation * glm::vec3(0, 0, 1);
			return InTransform->Forward;
		}

		//
		// Calculates the right direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateRight(Transform* InTransform)
		{
			if (InTransform == nullptr)
				return vec3(0.0f);

			InTransform->Right =  InTransform->Rotation * glm::vec3(1, 0, 0);
			return InTransform->Right;
		}


	};

}