#include "imgui_editor_types.h"


namespace Dawn
{
	EditorSceneData g_SceneData;
	EditorResources g_EditorResources;

	EditorSceneData* Editor_GetSceneData()
	{
		return &g_SceneData;
	}

	EditorResources* Editor_GetResources()
	{
		return &g_EditorResources;
	}

}