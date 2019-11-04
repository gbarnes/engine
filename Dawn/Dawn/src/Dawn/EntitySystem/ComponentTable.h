#pragma once
#include "inc_common.h"
#include "Component.h"
#include "Entity.h"

namespace Dawn
{
#define MAX_NUM_OF_COMPONENTS 4096

	class DAWN_API BaseComponentTable
	{
	public:
		BaseComponentTable() = default;
		virtual ~BaseComponentTable() = default;
		BaseComponentTable(const BaseComponentTable &) = default;
		BaseComponentTable &operator=(const BaseComponentTable &) = default;
		BaseComponentTable(BaseComponentTable &&) = default;
		BaseComponentTable &operator=(BaseComponentTable &&) = default;

		void SetTypeName(std::string InName) {
			TypeName = InName;
		}

		std::string GetTypeName() {
			return TypeName;
		}

		virtual bool ComponentForEntityExists(const Entity& InId) {
			return false;
		}

		virtual void ReleaseComponents()
		{

		}

	private:
		std::string TypeName;
	};

	template<typename T>
	class DAWN_API ComponentTable : public BaseComponentTable
	{
		typedef i32 EntityRawId;
		typedef u32 ComponentRawId;

	public:
		
		ComponentId Add(const Entity& InEntity, T* InComponent)
		{
			if (CurrentId == Components.max_size() - 1)
				return INVALID_HANDLE;

			ComponentId& id = ComponentIds[CurrentId];
			id.Index = CurrentId;
			id.IsValid = true;
			id.Generation += 1;
			id.Entity = InEntity;

			Component<T>* baseComponent = static_cast<Component<T>*>(InComponent);
			baseComponent->Id = id;

			Components[CurrentId] = InComponent;
			EntityToComponent[InEntity.Id] = CurrentId;
			
			u32 TempId = CurrentId;
			CurrentId = TotalCount + 1;

			if (TotalCount >= TempId)
				TotalCount++;

			return id;
		}

		void Destroy(const Entity& InEntity)
		{
			auto it = EntityToComponent.find(InEntity.Id);
			if (it == EntityToComponent.end())
				return nullptr;

			ComponentRawId rawid = it->second;

			if (Components[InEntity.Id] != nullptr)
			{
				delete Components[InEntity.Id];
				Components[InEntity.Id] = nullptr;
			}

			EntityToComponent.erase(InEntity.Id);

			ComponentId& id = ComponentIds[rawid];
			id.IsValid = false;
			CurrentId = rawId;
		}


		T* GetByEntityId(const Entity& InEntity)
		{
			auto it = EntityToComponent.find(InEntity.Id);
			if (it == EntityToComponent.end())
				return nullptr;

			ComponentRawId rawId = it->second;
			ComponentId id = ComponentIds[rawId];
			if (!id.IsValid)
				return nullptr;

			return Components[id.Index];
		}

		T* GetById(const ComponentId& InId)
		{
			ComponentId currentId = ComponentIds[InId.Index];

			if (!currentId.IsValid)
				return nullptr;

			if (currentId.Generation > InId.Generation) {

				return nullptr;
			}

			return Components[InId.Index];
		}

		bool ComponentForEntityExists(const Entity& InId) 
		{
			auto it = EntityToComponent.find(InId.Id);
			return (it != EntityToComponent.end());
		}

		std::vector<T*> GetComponents()
		{
			// this might be very slow...
			std::vector<T*> components;

			for (auto component : Components) {
				if (component == nullptr)
					continue;
				components.emplace_back(component);
			}

			return components;
		}

		void ReleaseComponents()
		{
			for (auto component : Components) {
				delete component;
				component = nullptr;
			}
		}

	protected:
		u32 TotalCount = 0;
		u32 CurrentId = 0;
		// mapping one entityid to one component id means that we only can have one component of this type per entity!
		std::map<EntityRawId, ComponentRawId> EntityToComponent; 
		std::array<ComponentId, MAX_NUM_OF_COMPONENTS> ComponentIds; 
		std::array<T*, MAX_NUM_OF_COMPONENTS> Components;
	};
}