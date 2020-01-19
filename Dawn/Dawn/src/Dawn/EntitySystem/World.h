#pragma once

#include "Component.h"
#include "ComponentTable.h"
#include "Entity.h"
#include "System.h"
#include "Core/Type.h"


#define MAX_NUM_OF_COMPONENT_TYPES 48
#define MAX_NUM_OF_SYSTEM_TYPES 48
#define COMP_INIT_LOAD_DEL(name, type, table) table.insert(std::make_pair<std::string, ComponentInitFunc>(std::string(name), std::bind(&##type##::InitFromLoad, std::placeholders::_1, std::placeholders::_2)));


namespace Dawn
{
	class BaseComponentTable;
	struct Camera;
	class World;

	typedef std::function<void(World*,void*)> ComponentInitFunc;

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
		void AddCamera(Camera* Cam);
		Camera* GetCamera(i32 InId);
		std::vector<Camera*> GetCameras();

		i32 CreateModelEntity(std::string& InName, std::string& InModelName, vec3& InPosition, vec3& InScale = vec3(1), quat& InRotation = quat());

		Entity CreateEntity(const std::string &InName);
		void DestroyEntity(const Entity& InEntity);
		EntityMetaData* GetEntityMetaData(const Entity& InEntity);
		void Shutdown();
		void ExecuteComponentInitFunc(void* InComponent, const std::string& InString);
		void UpdateSystems();
	public:
		template <typename T>
		void AddTable(std::string&& InTypeName, std::unique_ptr<ComponentTable<T>>&& InTable)
		{
			int index = Component<T>::GetTableIndex();
			
			INIT_TYPE(T);
			COMP_INIT_LOAD_DEL(InTypeName.c_str(), T, this->InitFuncTable);

			InTable.get()->SetTypeName(InTypeName);

			if(!ComponentTables[index])
				ComponentTables[index] = std::move(InTable);
		}

		template<typename T>
		T* MakeComponent() {

			T* comp = new T();
			Component<T>* baseComp = static_cast<Component<T>*>(comp);
			if (baseComp)
				baseComp->WorldRef = this;

			return comp;
		}


		template<typename T>
		ComponentId AddComponent(const Entity& InEntity, T* InComponent)
		{
			ComponentTable<T>* table = GetTable<T>();
			return table->Add(InEntity, InComponent);
		}

		ComponentId AddComponentByString(const Entity& InEntity, const std::string& InType, void* InComponent)
		{
			auto Table = GetTableByString(InType);
			return Table->AddByVoid(InEntity, InComponent);
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

			return static_cast<T*>(it->second);
		}

		void* GetComponentByName(const Entity& InEntity, const std::string& InName);
		void AddSystem(ISystem* InSystem);
		std::array<Entity, MaxNumbersOfEntities>& GetEntities(u32* OutCount);
		static Camera* GetActiveCamera();
		static void SetActiveCamera(Camera* InCamera);
		static bool LoadLevel(World* InWorld, struct Level* InLevel);

	private:
		static Camera* ActiveCamera;
		EntityTable Entities;
		std::vector<i32> CameraEntities;
		std::array<std::unique_ptr<BaseComponentTable>, MAX_NUM_OF_COMPONENT_TYPES> ComponentTables;
		std::map<std::string, ISystem*> SystemTable;
		std::map<std::string, ComponentInitFunc> InitFuncTable;

		template<typename T>
		ComponentTable<T>* GetTable()
		{
			u32 index = Component<T>::GetTableIndex();

			if (!ComponentTables[index])
				ComponentTables[index] = std::move(std::make_unique<ComponentTable<T>>(ComponentTable<T>()));

			return static_cast<ComponentTable<T>*>(ComponentTables[index].get());
		}

		BaseComponentTable* GetTableByString(const std::string& InName);
	};
}