#include "imgui_editor_assets.h"
#include "imgui_editor_types.h"
#include "ResourceSystem/Resources.h"
#include "EntitySystem/World.h"

namespace  Dawn
{
	void Editor_SaveScene(const EditorSceneData* InSceneData, World* InWorld)
	{
		Scene scene;
		scene.Name = InSceneData->Name.c_str();
		
		u32 count = 0;
		auto entities = EntityTable::GetEntities(&count);

		for (u32 i = 0; i < count; ++i)
		{
			auto entity = entities[i];
			auto meta = EntityTable::GetMeta(entity);
			EntityData data = {};
			data.Name = meta->Name.c_str();
			
			auto components = InWorld->GetComponentTypesByEntity(entity);
			for (auto component : components)
			{
				auto type = Type::GetType(component);

			//	type->GetMembers();
			}
			//data.IdToComponent.push_back();

			scene.Entities.push_back(data);
		}

	}
}