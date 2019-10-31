#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"
#include "Core/GDI/inc_gfx_types.h"
#include "inc_common.h"
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
		Entity* CurrentSelectedEntity = nullptr;
		ImDrawList* GizmoDrawList;
		vec3 LastEulerRotation;
		ImGuizmo::MODE EditSpace = ImGuizmo::WORLD;
		ImGuizmo::OPERATION EditMethod = ImGuizmo::TRANSLATE;
	};

	struct EditorResources
	{
		GfxResId Icons[EditorIcons::EI_Num];
		bool bIsLoaded = false;
	};

	EditorSceneData* Editor_GetSceneData();
	EditorResources* Editor_GetResources();
	

};