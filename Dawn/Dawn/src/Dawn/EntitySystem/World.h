#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Component.h"
#include "ComponentTable.h"
#include "Entity.h"
#include "System.h"


namespace Dawn
{
#define WorldLocatorId std::string("World")
#define MAX_NUM_OF_COMPONENT_TYPES 48
#define MAX_NUM_OF_SYSTEM_TYPES 48

	class BaseComponentTable;
	struct Camera;

	class DAWN_API World : public std::enable_shared_from_this<World>
	{
	public:
		World();
		~World();

	public:
		std::vector<std::string> GetComponentTypesByEntity(const Entity& InEntity);
		
		Camera* CreateCamera(std::string& InName, u32 Width, u32 Height,
			float InNearZ, float InFarZ, float InFoV, vec4 InClearColor, 
			const vec3& InPosition = vec3(0), const quat& InOrientation = quat());
		
		Camera* GetCamera(i32 InId);
		std::vector<Camera*> GetCameras();

		Entity CreateEntity(const std::string &InName);
		EntityMetaData* GetEntityMetaData(const Entity& InEntity)
		{
			return Entities.GetMeta(InEntity);
		}
		void Shutdown();

	public:
		template <typename T>
		void AddTable(std::string&& InTypeName, std::unique_ptr<ComponentTable<T>>&& InTable)
		{
			int index = Component<T>::GetTableIndex();
			
			INIT_TYPE(T);

			InTable.get()->SetTypeName(InTypeName);

			if(!ComponentTables[index])
				ComponentTables[index] = std::move(InTable);
		}

		template<typename T>
		T* MakeComponent() {
			return new T;
		}


		template<typename T>
		ComponentId AddComponent(const Entity& InEntity, T* InComponent)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->Add(InEntity, InComponent);
		}

		template<typename T>
		T* GetComponentByEntity(const Entity& InEntity)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->GetByEntityId(InEntity);
		}

		template<typename T>
		T* GetComponentById(const ComponentId& InComponent)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->GetById(InComponent);
		}

		template<typename T>
		std::vector<T*> GetComponentsByType()
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->GetComponents();
		}
		
		template<typename T, typename V>
		std::map<T*, V*> GetComponentSets()
		{
			std::map<T*, V*> map;

			auto componentsPrimaries = GetComponentsByType<T>();
			for (auto left : componentsPrimaries)
			{
				V* right = GetComponentByEntity<V>(left->Id.Entity);
				if(left)
					map.insert(std::make_pair(left, right));
			}

			return map;
		}

		template<typename T>
		T* GetSystemByType(DawnType* InType)
		{
			auto it = SystemTable.find(InType->GetName());
			if (it == SystemTable.end())
				return nullptr;

			return static_cast<T*>(it->second.get());
		}

		void* GetComponentByName(const Entity& InEntity, const std::string& InName)
		{
			BaseComponentTable* ComponentTable = nullptr;

			for (auto& table : ComponentTables)
			{
				if (table->GetTypeName() == InName)
				{
					ComponentTable = table.get();
					break;
				}
			}

			return ComponentTable->GetComponentByEntity(InEntity);
			//it->second->
		}

		void AddSystem(std::unique_ptr<ISystem> InSystem)
		{
			auto type = InSystem->AccessType();
			auto it = SystemTable.find(type->GetName());
			if (it != SystemTable.end())
				return;

			SystemTable.insert(std::make_pair(type->GetName(), std::move(InSystem)));
		}

		std::array<Entity, MaxNumbersOfEntities>& GetEntities(u32* OutCount)
		{
			return Entities.GetEntities(OutCount);
		}

		static Camera* GetActiveCamera()
		{
			return ActiveCamera;
		}

		static void SetActiveCamera(Camera* InCamera)
		{
			ActiveCamera = InCamera;
		}

	private:
		static Camera* ActiveCamera;
		EntityTable Entities;
		std::vector<i32> CameraEntities;
		std::array<std::unique_ptr<BaseComponentTable>, MAX_NUM_OF_COMPONENT_TYPES> ComponentTables;
		std::map<std::string, std::unique_ptr<ISystem>> SystemTable;

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