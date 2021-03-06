#pragma once
#include "Core/Container/HandleObjectArray.h"
#include "Core/Memory/MemoryUtils.h"
#include "Resources.h"

namespace Dawn
{
	class File;
	class ResourceSystem;
	struct ResourceWatchData;

	struct DAWN_API FileMetaData
	{
		FileId Id;
		std::string Name;
		std::string Path;
		std::string Ext;
		u64 Size;
	};


	typedef std::map<FileId, FileMetaData> FileMetaDatabase;
	typedef std::function<ResourceId(ResourceSystem* InFS, const std::string&, FileMetaData*) > FileLoadDelegate;
	 

	struct DAWN_API FileLoaderDelegates
	{
		FileLoadDelegate OnLoadDelegate;
		FileLoadDelegate OnReloadDelegate;
	};

	static FileId CreateHashFromFileName(std::string InFilename) {
		return std::hash<std::string>()(InFilename);
	}

	#define CREATE_FILE_HANDLE(x) CreateHashFromFileName(x)
	#define BIND_FS_LOADER(x) std::bind(&x, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

	#define DECLARE_RESOURCE_TYPE(typeName, className, tableName) className##* Find##className##(const ResourceId& InId) {\
	return FindResourceFromId(InId, &Resources.##tableName##);\
	}\
	ResourceId Create##className##(##className##** OutResource){\
	return CreateResource<##className##>(##typeName##, &Resources.##tableName##, OutResource);\
	}\
	std::vector<##className##*> Get##tableName##() {\
		return GetAllResourcesOfType(&Resources.##tableName##);\
	}

	class DAWN_API ResourceSystem : public std::enable_shared_from_this<ResourceSystem>
	{
	public:
		static Shared<ResourceSystem> Create(Path InPath, std::vector<std::string> InFilter, bool InWatchForChanges = false)
		{ 
			return Shared<ResourceSystem>(new ResourceSystem(InPath, InFilter, InWatchForChanges));
		}

		~ResourceSystem();

		// Setup Phase
	public:
		void Refresh();
		void Shutdown();
		bool BuildDatabase();
		void RegisterLoader(const FileLoadDelegate& InLoadDelegate, const FileLoadDelegate& InReloadDelegate, const std::list<std::string> & InExtensions);

		// File loading api
	public:
		FileMetaData* GetMetaDataFromHandle(FileId InHandle);
		ResourceId LoadFile(const std::string& InFilename);
		ResourceId LoadFile(FileId InHandle);
		std::vector<FileMetaData> GetAllMetaFiles();
		FileId GetFileId(const std::string& InFilename);

		// Resource functions
	public:
		ResourceId FindResourceIdFromFileId(FileId InResource);

		DECLARE_RESOURCE_TYPE(ResourceType_StaticMesh, Mesh, Meshes)
		DECLARE_RESOURCE_TYPE(ResourceType_Model, Model, Models)
		DECLARE_RESOURCE_TYPE(ResourceType_Material, Material, Materials)
		DECLARE_RESOURCE_TYPE(ResourceType_Image, Image, Images)
		DECLARE_RESOURCE_TYPE(ResourceType_Shader, Shader, Shaders)
		DECLARE_RESOURCE_TYPE(ResourceType_Level, Level, Levels)
	private:
		ResourceSystem(Path InPath, std::vector<std::string> InFilter, bool bWatchForChanges);

		std::vector<std::string> Filters;

		struct ResourceDatabase
		{
			HandleObjectArray<Mesh> Meshes;
			HandleObjectArray<Model> Models;
			HandleObjectArray<Shader> Shaders;
			HandleObjectArray<Image> Images;
			HandleObjectArray<Material> Materials;
			HandleObjectArray<Level> Levels;
		};

		bool bWatchForChanges;
		Unique<ResourceWatchData> ResourcesWatchData;
		ResourceDatabase Resources;
		std::map<FileId, ResourceId> FileIdToResourceId;
		std::map<std::string, FileLoaderDelegates> FileLoaders;
		
		std::string WorkingSpace;
		FileMetaDatabase MetaDatabase;

		bool IsValidFilter(std::string& InExtension);


		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
		ResourceId CreateResource(ResourceType InType, HandleObjectArray<T>* InArray, T** OutResource)
		{
			const auto Slot = InArray->Request();
			Allocators::LinearAllocator* Arena = InArray->GetMemArena();

			T* NewResource = new (Arena->Allocate(sizeof(T), __alignof(T), 0)) T(); //D_NEW_ALLOC(T, 0, Arena)();

			Resource* BaseResource = static_cast<Dawn::Resource*>(NewResource);
			BaseResource->Id = Slot->GetId();
			BaseResource->Type = InType;

			Slot->Element = NewResource;
			*OutResource = NewResource;

			return Slot->GetId();
		}

		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
		T* FindResourceFromId(const ResourceId& InId, HandleObjectArray<T>* InArray)
		{
			assert(InId.IsValid);
			return InArray->Find(InId);
		}

		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
		std::vector<T*> GetAllResourcesOfType(HandleObjectArray<T>* InArray)
		{
			assert(InArray);
			return InArray->GetValues();
		}
	};

}