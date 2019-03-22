#pragma once
#include "inc_core.h"
#include "inc_common.h"

#include "Resources.h"

namespace Dawn
{
	class File;
	class ResourceSystem;
	#define FileSystemId std::string("FS")
	
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
		ResourceSystem(std::string InPath, std::vector<std::string> InFilter);
		~ResourceSystem();

		inline static RefPtr<ResourceSystem> Get()
		{
			return RefPtr<ResourceSystem>(Locator::Get<ResourceSystem>(FileSystemId));
		}

		bool BuildDatabase();
		void RegisterLoader(ResourceType InType, FileLoadDelegate InDelegate);
		FileMetaData* GetMetaDataFromHandle(FileHandle InHandle);

		GenericHandle LoadFile(FileHandle InHandle);

		template<typename T>
		static bool RegisterResource(ResourceType InType, T* InResource)
		{
			if (InType == ResourceType_StaticMesh) {

				Mesh* mesh = static_cast<Mesh*>(InResource);
				Resources.Meshes.emplace_back(*mesh);
			}

			return false;
		}

		static Mesh* GetMesh(MeshHandle InHandle)
		{
			u32 index = InHandle.Index;
			if (Resources.Meshes.size() < index)
				return nullptr;

			return &Resources.Meshes[index];
		}

		static u128 ResourceAmount(ResourceType InType);

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