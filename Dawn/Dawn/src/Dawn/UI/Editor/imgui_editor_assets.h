#pragma once

namespace Dawn
{
	struct EditorSceneData;
	class World;
	class ResourceSystem;

	void Editor_SaveLevel(Path InPath, EditorSceneData* InSceneData, World* InWorld, ResourceSystem* InResources);

	//Path Editor_OpenFileDialog(const Path& InPath, std::initializer_list<std::string> InExtensions);
}