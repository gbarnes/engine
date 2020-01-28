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
		transform->CalculateLocalSpace();

		TransformUtils::CalculateForward(transform);
		TransformUtils::CalculateUp(transform);
		TransformUtils::CalculateRight(transform);

		SceneObject Obj;
		Obj.TransformId = transform->Id;
		
		auto* Node = InWorld->GetScene()->AddNode(Obj);
		D_ASSERT(Node != nullptr, "Couldn't create node for transform?!");

		transform->SceneIndex = Node->DataIndex;
	}

	Transform* Transform::GetParent(World* InWorld)
	{
		auto* Object = InWorld->GetScene()->GetParentDataFromIndex(SceneIndex);
		if (!Object) return nullptr;
		return InWorld->GetComponentById<Transform>(Object->TransformId);
	}

	void Transform::SetParent(World* InWorld, Transform* InParent)
	{
		auto* Scene = InWorld->GetScene();
		Scene->SetParent(SceneIndex, (InParent == nullptr) ? -1 : InParent->SceneIndex);
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