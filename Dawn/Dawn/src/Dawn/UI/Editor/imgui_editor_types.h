#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"
#include "Core/GDI/inc_gfx_types.h"
#include "inc_common.h"
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
		vec3 LastEulerRotation;
		ImGuizmo::MODE EditSpace = ImGuizmo::WORLD;
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