#pragma once
#include "inc_common.h"
#include "Entity.h"

namespace Dawn
{
	struct ComponentId : public GenericHandle
	{
		EntityId Entity;
	};

	struct ComponentIndexCounter
	{
		static u32 TableCounter; // the global index counter for all components
	};

	template <typename T>
	struct DAWN_API Component
	{
		// 
		// Returns the index for this component type.
		// 
		static inline u32 GetTableIndex()
		{
			static u32 index = ComponentIndexCounter::TableCounter++;
			return index;
		}

		ComponentId Id;

		const Entity* Component::GetEntity()
		{
			return EntityTable::Get(Id.Entity);
		}
	};
}