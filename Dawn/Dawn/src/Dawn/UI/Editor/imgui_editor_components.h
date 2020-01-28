#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"

namespace Dawn
{
	struct Entity;
	struct Transform;
	struct Camera;
	struct DirectionalLight;
	struct PointLight;
	struct MeshFilter;
	class ResourceSystem;
	class World;

	void ShowEntity(World* InWorld, Entity* InEntity);
	void ShowTransformComponent(World* InWorld, Transform* InTransform, ImGuizmo::MODE& InEditSpace);
	void ShowCameraComponent(Camera* InCamera);
	void ShowDirectionalLightComponent(DirectionalLight* InLight);
	void ShowPointLightComponent(PointLight* InLight);
	void ShowMeshFilterComponent(MeshFilter* InMeshFilter, ResourceSystem* InResourceSystem);
}