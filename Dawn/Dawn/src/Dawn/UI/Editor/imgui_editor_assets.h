#pragma once


namespace Dawn
{
	struct EditorSceneData;
	class World;

	void Editor_SaveScene(const EditorSceneData* InSceneData, World* InWorld);
}