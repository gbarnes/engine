#pragma once
#include "inc_common.h"
#include "Entity.h"

namespace Dawn
{
#define MAX_NUM_OF_ENTITIES 4096  // abritary number of aligned max entities for now?! 

	class EntityTable
	{
	public:
		static Entity* Get(const EntityId& InId);
		static EntityId Create(const std::string& InName);
		static void Remove(const EntityId& InId);
	private:
		static u32 CurrentId;
		static u32 TotalCount;
		static std::array<Entity, MAX_NUM_OF_ENTITIES> Entities;
		static std::array<EntityId, MAX_NUM_OF_ENTITIES> EntityIds;
	};
}