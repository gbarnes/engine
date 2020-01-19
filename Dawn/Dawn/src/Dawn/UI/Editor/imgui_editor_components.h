#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"

namespace Dawn
{
	struct Entity;
	struct Transform;
	struct Camera;
	struct DirectionalLight;
	struct PointLight;
	struct ModelView;
	class ResourceSystem;
	class World;

	void ShowEntity(World* InWorld, Entity* InEntity);
	void ShowTransformComponent(Transform* InTransform, ImGuizmo::MODE& InEditSpace);
	void ShowCameraComponent(Camera* InCamera);
	void ShowDirectionalLightComponent(DirectionalLight* InLight);
	void ShowPointLightComponent(PointLight* InLight);
	void ShowModelViewComponent(ModelView* InModelView, ResourceSystem* InResourceSystem);
}