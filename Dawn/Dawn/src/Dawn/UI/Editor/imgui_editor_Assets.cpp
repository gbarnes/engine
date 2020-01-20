#include "stdafx.h"
#include "imgui_editor_assets.h"
#include "imgui_editor_types.h"
#include "Core/Logging/Log.h"
#include "Core/Type.h"
#include "ResourceSystem/Resources.h"
#include "EntitySystem/World.h"
#include "ResourceSystem/ResourceSystem.h"

namespace  Dawn
{
	//
	// Looks for the given scene at the path. If it not exists it 
	// will create a new scene.
	//
	Level* GetLevel(ResourceSystem* InResources, const std::string& InPath)
	{
		auto resId = InResources->LoadFile(InPath);
		if (!resId.IsValid) {
			Level* Level = nullptr;
			InResources->CreateLevel(&Level);
			return Level;
		}

		return InResources->FindLevel(resId);
	}

	void Editor_SaveLevel(Path InPath, EditorSceneData* InSceneData, World* InWorld, ResourceSystem* InResources)
	{
		Level* level = GetLevel(InResources, InSceneData->Name);
		level->Name = InSceneData->Name.c_str();
		level->Components.clear();
		level->Entities.clear();

		FILE* LevelFile;
		i32 errorno = fopen_s(&LevelFile, InPath.append(std::string(level->Name) + ".level").string().c_str(), "w");
		if (errorno > 0)
			return; // todo --- error handling.

		u32 count = 0;
		auto entities = InWorld->GetEntities(&count);
		int id = 0;

		for (u32 i = 0; i < count; ++i)
		{
			auto entity = entities[i];
			if (!entity.IsValid())
				continue;

			auto meta = InWorld->GetEntityMetaData(entity);
			
			if (meta->bIsHiddenInEditorHierarchy)
				continue;
			
			EntityData data = {};
			data.Name = meta->Name.c_str();

			fputs(std::string("---Entity "+ data.Name + "\n").c_str(), LevelFile);
			
			auto componentNames = InWorld->GetComponentTypesByEntity(entity);
			for (const auto& componentName : componentNames)
			{
				ComponentData compData = {};
				compData.Type = componentName.c_str();
				compData.Id = id;

				auto type = DawnType::GetType(componentName);
				auto members = type->GetMembers();
				auto componentInstance = InWorld->GetComponentByName(entity, componentName);
				
				fputs(std::string("---Component " + componentName + "\n").c_str(), LevelFile);

				for (auto& member : members)
				{
					if (!member.bSerialize)
						continue;

					ComponentData::ComponentValue Value = {};
					Value.VariableName = member.Name;
					compData.ComponentValues.push_back(Value);

					fputs(("  " + member.Name + ": " + member.Type->ToString(type->GetAsVoid(componentInstance, member.Name)).append("\n")).c_str(), LevelFile);
					
					DWN_CORE_INFO("Member: {0}", member.Name.c_str());
				}

				data.IdToComponent.push_back(compData.Id);
				level->Components.push_back(compData);

				++id;
			}

			level->Entities.push_back(data);
		}

		fclose(LevelFile);
	}
}