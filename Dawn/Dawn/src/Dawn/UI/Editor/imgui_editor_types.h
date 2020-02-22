#pragma once
#include "Vendor/ImGuizmo/ImGuizmo.h"
#include "Core/GDI/Base/inc_gfx_types.h"
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

	struct EditorSelectionData
	{
		Entity Entity = INVALID_ENTITY;
		mat4 ModelMatrix;
		vec3 Position;
		vec3 Scale;
		vec3 Rotation;
	};
	struct EditorSceneData
	{
		EditorSelectionData SelectionData;
		ImDrawList* GizmoDrawList;
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