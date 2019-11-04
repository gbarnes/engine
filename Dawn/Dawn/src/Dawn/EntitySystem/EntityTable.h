#pragma once
#include "inc_common.h"
#include "Entity.h"

namespace Dawn
{
	constexpr int MaxNumbersOfEntities = 4096;  // abritary number of aligned max entities for now?! 

	class EntityTable
	{
	public:
		static Entity Get(const i32 InId);
		static EntityMetaData* GetMeta(const Entity& InEntity);
		static Entity Create(const std::string& InName);
		static void Remove(const i32 InId);
		static std::array<Entity, MaxNumbersOfEntities>& GetEntities(u32* OutCount);
	private:
		static u32 CurrentId;
		static u32 TotalCount;
		static std::array<Entity, MaxNumbersOfEntities> Entities;
		static std::array<EntityMetaData, MaxNumbersOfEntities> MetaData;
	};
}