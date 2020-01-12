#include "EntityTable.h"

namespace Dawn
{
	Entity EntityTable::Get(const i32 InId)
	{
		return Entities[InId];
	}

	EntityMetaData* EntityTable::GetMeta(const Entity& InEntity)
	{
		return &MetaData[InEntity.Id];
	}

	Entity EntityTable::Create(const std::string& InName)
	{
		if (CurrentId == Entities.max_size() - 1)
			return Entity();

		Entity& NewHandle = Entities[CurrentId];
		NewHandle.Id = CurrentId;
		NewHandle.Generation += 1;

		EntityMetaData& Meta = MetaData[CurrentId];
		Meta.bIsActive = true;
		Meta.bIsHiddenInEditorHierarchy = false;
		Meta.Name = InName;
		Meta.World = this->World;

		u32 TempId = CurrentId;
		CurrentId = TotalCount + 1;
		
		if (TotalCount >= TempId)
			TotalCount++;

		return NewHandle;
	}

	void EntityTable::Remove(const i32 InId)
	{
		Entities[InId].Id = -1;
		CurrentId = InId;
	}

	std::array<Entity, MaxNumbersOfEntities>& EntityTable::GetEntities(u32* OutCount)
	{
		*OutCount = TotalCount;
		return Entities;
	}
}