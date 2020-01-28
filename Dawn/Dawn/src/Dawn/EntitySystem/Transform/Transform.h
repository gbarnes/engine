#pragma once
#include "../Component.h"
#include "Core/Type.h"
#include "EntitySystem/World.h"

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

		i32 SceneIndex;

		mat4 WorldSpace = mat4(1);
		mat4 LocalSpace = mat4(1);

		static void InitFromLoad(World* InWorld, void* Component);

		Transform* GetParent(World* InWorld);
		void SetParent(World* InWorld, Transform* InParent);

		void CalculateLocalSpace() {
			LocalSpace =  glm::translate(mat4(1), this->Position) * glm::toMat4(this->Rotation) *  glm::scale(mat4(1), this->Scale);
		}

		mat4 GetInverseTransform() {
			vec3 invertedPos = -1.0f*Position;
			return glm::translate(mat4(1), invertedPos)  *  glm::scale(mat4(1), 1.0f / Scale) * glm::transpose(glm::toMat4(Rotation));
		}

		vec3 GetWorldPosition() {
			return WorldSpace * vec4(Position, 1);
		}

		vec3 GetWorldScale() {
			return WorldSpace * vec4(Scale, 1);
		}

		
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
		static mat4 GetRotationMatrix(Transform* InTransform);

		//
		// 
		//
		static void Rotate(Transform* InTransform, const quat& Rotation);

		//
		// Calculates the up direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateUp(Transform* InTransform);

		//
		// Calculates the forward direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateForward(Transform* InTransform);

		//
		// Calculates the right direction vector for the transform 
		// by the currently given orientation.
		//
		static vec3 CalculateRight(Transform* InTransform);
	};

}