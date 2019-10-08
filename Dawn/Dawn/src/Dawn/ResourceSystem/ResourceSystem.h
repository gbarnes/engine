#pragma once
#include "inc_core.h"
#include "inc_common.h"
#include "Core/Container/HandleObjectArray.h"
#include "Resources.h"

namespace Dawn
{
	class File;
	class ResourceSystem;

	struct DAWN_API FileMetaData
	{
		FileId Id;
		std::string Name;
		std::string Path;
		std::string Ext;
		u128 Size;
	};

	typedef std::map<FileId, FileMetaData> FileMetaDatabase;
	typedef std::function<ResourceId(ResourceSystem* InFS, std::string&, FileMetaData*) > FileLoadDelegate;

	static FileId CreateHashFromFileName(std::string InFilename) {
		return std::hash<std::string>()(InFilename);
	}

	#define CREATE_FILE_HANDLE(x) CreateHashFromFileName(x)
	#define BIND_FS_LOADER(x) std::bind(&x, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

	#define DECLARE_RESOURCE_TYPE(typeName, className, tableName) className##* Find##className##(const ResourceId& InId) {\
	return FindResourceFromId(InId, &Resources.##tableName##);\
	}\
	ResourceId Create##className##(##className##** OutResource){\
	return CreateResource(##typeName##, &Resources.##tableName##, OutResource);\
	}

	class DAWN_API ResourceSystem
	{
	public:
		static Shared<ResourceSystem> Create(std::string InPath, std::vector<std::string> InFilter)
		{
			return std::shared_ptr<ResourceSystem>(new ResourceSystem(InPath, InFilter));
		}

		~ResourceSystem();

		// Setup Phase
	public:
		void Shutdown();
		bool BuildDatabase();
		void RegisterLoader(FileLoadDelegate InDelegate, const std::list<std::string> & InExtensions);

		// File loading api
	public:
		FileMetaData* GetMetaDataFromHandle(FileId InHandle);
		ResourceId LoadFile(std::string InFilename);
		ResourceId LoadFile(FileId InHandle);
		std::vector<FileMetaData> GetAllMetaFiles();

		// Resource functions
	public:
		ResourceId FindResourceIdFromFileId(FileId InResource);

		DECLARE_RESOURCE_TYPE(ResourceType_StaticMesh, Mesh, Meshes)
		DECLARE_RESOURCE_TYPE(ResourceType_Model, Model, Models)
		DECLARE_RESOURCE_TYPE(ResourceType_Material, Material, Materials)
		DECLARE_RESOURCE_TYPE(ResourceType_Image, Image, Images)
		DECLARE_RESOURCE_TYPE(ResourceType_Shader, Shader, Shaders)
	private:
		ResourceSystem(std::string InPath, std::vector<std::string> InFilter);

		std::vector<std::string> Filters;

		struct ResourceDatabase
		{
			HandleObjectArray<Mesh> Meshes;
			HandleObjectArray<Model> Models;
			HandleObjectArray<Shader> Shaders;
			HandleObjectArray<Image> Images;
			HandleObjectArray<Material> Materials;
		};

		ResourceDatabase Resources;
		std::map<FileId, ResourceId> FileIdToResourceId;
		std::map<std::string, FileLoadDelegate> FileLoaders;
		
		std::string WorkingSpace;
		FileMetaDatabase MetaDatabase;

		bool IsValidFilter(std::string& InExtension);


		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
		ResourceId CreateResource(ResourceType InType, HandleObjectArray<T>* InArray, T** OutResource)
		{
			const auto Slot = InArray->Request();
			
			T* NewResource = new T();

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
	};

}