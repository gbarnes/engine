#pragma once

namespace Dawn
{
	struct Entity;
	struct Transform;
	struct Camera;

	void ShowEntity(Entity* InEntity);
	void ShowTransformComponent(Transform* InTransform);
	void ShowCameraComponent(Camera* InCamera);
}