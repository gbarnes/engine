#include "stdafx.h"
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


	
	mat4 TransformUtils::GetRotationMatrix(Transform* InTransform)
	{
		if (InTransform == nullptr)
			return mat4(1.0f);

		return glm::mat4_cast(InTransform->Rotation);
	}

	void TransformUtils::Rotate(Transform* InTransform, const quat& Rotation)
	{
		InTransform->Rotation = Rotation;
	}

	vec3 TransformUtils::CalculateUp(Transform* InTransform)
	{
		if (InTransform == nullptr)
			return vec3(0.0f);

		InTransform->Up = glm::normalize(InTransform->Rotation * glm::vec3(0, 1, 0));
		return InTransform->Up;
	}

	vec3 TransformUtils::CalculateForward(Transform* InTransform)
	{
		if (InTransform == nullptr)
			return vec3(0.0f);

		InTransform->Forward = glm::normalize(InTransform->Rotation * glm::vec3(0, 0, 1));
		return InTransform->Forward;
	}

	vec3 TransformUtils::CalculateRight(Transform* InTransform)
	{
		if (InTransform == nullptr)
			return vec3(0.0f);

		InTransform->Right = glm::normalize(InTransform->Rotation * glm::vec3(1, 0, 0));
		return InTransform->Right;
	}
}