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
	struct Camera;

	class DAWN_API World : public EObject
	{
	public:
		World();
		~World();

		inline static RefPtr<World> Get()
		{
			return RefPtr<World>(Locator::Get<World>("World"));
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

		template<typename T>
		T* GetComponentByEntity(EntityId& InEntity)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->GetByEntityId(InEntity);
		}

		template<typename T>
		T* GetComponentById(ComponentId& InComponent)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->GetById(InComponent);
		}

		Camera* CreateCamera(std::string& InName, vec3 InPosition, float InAspectRatio,
			float InNearZ, float InFarZ, float InFoV, vec4 InClearColor);

		Camera* GetCamera(u32 InId);

	private:
		std::vector<EntityId> CameraEntities;
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