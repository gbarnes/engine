#pragma once

namespace Dawn
{
	struct Entity;
	struct Transform;
	struct Camera;
	struct DirectionalLight;
	struct PointLight;

	void ShowEntity(Entity* InEntity);
	void ShowTransformComponent(Transform* InTransform);
	void ShowCameraComponent(Camera* InCamera);
	void ShowDirectionalLightComponent(DirectionalLight* InLight);
	void ShowPointLightComponent(PointLight* InLight);
}