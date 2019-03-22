#pragma once
#include "inc_core.h"
#include "inc_common.h"
#include "Resources.h"

namespace Dawn
{
	class File;
	class ResourceSystem;

	struct DAWN_API ResourceDatabase
	{
		std::vector<std::shared_ptr<Mesh>> Meshes;
		std::vector<std::shared_ptr<Material>> Materials;
		std::vector<std::shared_ptr<Shader>> Shaders;
		std::vector<std::shared_ptr<Image>> Images;
	};

	struct DAWN_API FileMetaData
	{
		FileHandle Id;
		std::string Name;
		std::string Path;
		std::string Ext;
		ResourceType Type;
		u128 Size;
	};


	#define ResourceSystemId std::string("FS")
	
	typedef std::map<FileHandle, FileMetaData> FileMetaDatabase;
	typedef std::function<GenericHandle(ResourceSystem* InFS, std::string&, FileMetaData*) > FileLoadDelegate;

	static FileHandle CreateHashFromFileName(std::string InFilename) {
		return std::hash<std::string>()(InFilename);
	}

	#define CREATE_FILE_HANDLE(x) CreateHashFromFileName(x)
	#define BIND_FS_LOADER(x) std::bind(&x, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

	
	class DAWN_API ResourceSystem : public EObject
	{
	public:
		ResourceSystem();
		~ResourceSystem();

		inline static RefPtr<ResourceSystem> Get()
		{
			return RefPtr<ResourceSystem>(Locator::Get<ResourceSystem>(ResourceSystemId));
		}

		bool Initialize(std::string InPath, std::vector<std::string> InFilter);
		void Shutdown();
		bool BuildDatabase();
		void RegisterLoader(ResourceType InType, FileLoadDelegate InDelegate);
		FileMetaData* GetMetaDataFromHandle(FileHandle InHandle);

		GenericHandle LoadFile(FileHandle InHandle);

		static std::shared_ptr<Mesh> GetMesh(MeshHandle InHandle)
		{
			u32 index = InHandle.Index;
			if (Resources.Meshes.size() < index)
				return nullptr;

			return Resources.Meshes[index];
		}

		static std::shared_ptr<Shader> GetShader(ShaderHandle InHandle)
		{
			u32 index = InHandle.Index;
			if (Resources.Shaders.size() < index)
				return nullptr;

			return Resources.Shaders[index];
		}

		static u128 ResourceCount(const ResourceType InType);
		static bool RegisterResource(ResourceType InType, void* InResource);
	private:
		std::vector<std::string> Filters;
		std::map<ResourceType, FileLoadDelegate> FileLoaders;
		std::string WorkingSpace;
		static ResourceDatabase Resources;
		FileMetaDatabase MetaDatabase;
	
		bool IsValidFilter(std::string& InExtension);
	};

	typedef RefPtr<ResourceSystem> FileSystemPtr;

}