#pragma once
#include "Entity.h"

namespace Dawn
{
	class World;
	constexpr int MaxNumbersOfEntities = 4096;  // abritary number of aligned max entities for now?! 

	class EntityTable
	{
	public:
		EntityTable(World* InWorld) 
			: World(InWorld) {}
		
		Entity Get(const i32 InId);
		EntityMetaData* GetMeta(const Entity& InEntity);
		EntityMetaData* GetMeta(const UUID& InUUID);
		Entity Create(const std::string& InName);
		void Remove(const i32 InId);
		std::array<Entity, MaxNumbersOfEntities>& GetEntities(u32* OutCount);
		void Clear()
		{
		}

	private:
		World* World;
		u32 CurrentId = 0;
		u32 TotalCount = 0;
		std::array<Entity, MaxNumbersOfEntities> Entities;
		std::array<EntityMetaData, MaxNumbersOfEntities> MetaData;
	};
}