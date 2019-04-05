#include "EntityTable.h"

namespace Dawn
{
	std::array<Entity, MAX_NUM_OF_ENTITIES> EntityTable::Entities;
	std::array<EntityId, MAX_NUM_OF_ENTITIES> EntityTable::EntityIds;
	u32 EntityTable::CurrentId = 0;
	u32 EntityTable::TotalCount = 0;

	const Entity* EntityTable::Get(const EntityId& InId)
	{
		EntityId CurrentId = EntityIds[InId.Index];
		if (CurrentId.Generation > InId.Generation) {

			return nullptr;
		}

		return &Entities[InId.Index];
	}

	EntityId EntityTable::Create(const std::string& InName)
	{
		if (CurrentId == EntityIds.max_size() - 1)
			return INVALID_HANDLE;

		EntityId& NewHandle = EntityIds[CurrentId];
		NewHandle.Index = CurrentId;
		NewHandle.Generation += 1;
		NewHandle.IsValid = true;

		EntityIds[CurrentId] = NewHandle;
		Entities[CurrentId] = { NewHandle, InName };

		u32 TempId = CurrentId;
		CurrentId = TotalCount + 1;
		
		if (TotalCount >= TempId)
			TotalCount++;

		return NewHandle;
	}

	void EntityTable::Remove(const EntityId& InId)
	{
		EntityIds[InId.Index].IsValid = false;
		CurrentId = InId.Index;
	}
}