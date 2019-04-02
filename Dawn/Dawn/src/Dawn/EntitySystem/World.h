#pragma once
#include "inc_common.h"
#include "inc_core.h"

#include "Component.h"
#include "ComponentTable.h"
#include "Entity.h"

namespace Dawn
{
#define WorldLocatorId std::string("World")
#define MAX_NUM_OF_COMPONENT_TYPES 48

	class BaseComponentTable;

	class DAWN_API World : public EObject
	{
	public:
		World() = default;

		inline static RefPtr<World> Get()
		{
			return RefPtr<World>(Locator::Get<World>(WorldLocatorId));
		}

		template <typename T>
		void AddTable(std::unique_ptr<ComponentTable<T>>&& InManager)
		{
			int index = Component<T>::GetTableIndex();

			if(!ComponentTables[index])
				ComponentTables[index] = std::move(InManager);
		}

		template<typename T>
		ComponentId AddComponent(EntityId& InEntity, T& InComponent)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->Add(InEntity, InComponent);
		}


	private:
		std::array<std::unique_ptr<BaseComponentTable>, MAX_NUM_OF_COMPONENT_TYPES> ComponentTables;
		
		template<typename T>
		ComponentTable<T>* GetTable()
		{
			u32 index = Component<T>::GetTableIndex();

			if (!ComponentTables[index])
				ComponentTables[index] = std::move(std::make_unique<ComponentTable<T>>(ComponentTable<T>()));

			return static_cast<ComponentTable<T>*>(ComponentTables[index].get());
		}
	};


}