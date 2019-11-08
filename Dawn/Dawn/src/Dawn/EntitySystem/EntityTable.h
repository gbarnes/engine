#pragma once
#include "inc_common.h"
#include "Entity.h"

namespace Dawn
{
	constexpr int MaxNumbersOfEntities = 4096;  // abritary number of aligned max entities for now?! 

	class EntityTable
	{
	public:
		EntityTable(Shared<World> InWorld) 
			: World(InWorld) {}
		
		Entity Get(const i32 InId);
		EntityMetaData* GetMeta(const Entity& InEntity);
		Entity Create(const std::string& InName);
		void Remove(const i32 InId);
		std::array<Entity, MaxNumbersOfEntities>& GetEntities(u32* OutCount);
	private:
		Shared<World> World;
		u32 CurrentId = 0;
		u32 TotalCount = 0;
		std::array<Entity, MaxNumbersOfEntities> Entities;
		std::array<EntityMetaData, MaxNumbersOfEntities> MetaData;
	};
}