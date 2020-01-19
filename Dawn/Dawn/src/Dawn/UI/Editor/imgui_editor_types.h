#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"
#include "Core/GDI/inc_gfx_types.h"
#include "EntitySystem/Entity.h"
#include "imgui.h"

namespace Dawn
{
	enum EditorIcons
	{
		EI_ScenePointLightIcon,
		EI_SceneDirLightIcon,
		EI_SceneCameraIcon,
		EI_Num
	};

	struct Entity;

	struct EditorSceneData
	{
		Entity CurrentSelectedEntity = INVALID_ENTITY;
		ImDrawList* GizmoDrawList;
		mat4 ModelMatrix;
		vec3 LastEulerRotation;
		ImGuizmo::MODE EditSpace = ImGuizmo::LOCAL;
		ImGuizmo::OPERATION EditMethod = ImGuizmo::TRANSLATE;
		std::string Name = "";
	};

	struct EditorResources
	{
		GfxResId Icons[EditorIcons::EI_Num];
		bool bIsLoaded = false;
	};

	EditorSceneData* Editor_GetSceneData();
	EditorResources* Editor_GetResources();
	

};