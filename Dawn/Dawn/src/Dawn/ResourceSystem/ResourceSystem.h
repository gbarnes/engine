#pragma once
#include "inc_core.h"
#include "inc_common.h"
#include "Resources.h"
#include "ResourceTable.h"

namespace Dawn
{
	class File;
	class ResourceSystem;

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

		GenericHandle LoadFile(std::string InFilename);
		GenericHandle LoadFile(FileHandle InHandle);

		std::vector<FileMetaData> GetAllMetaFiles();

	private:
		std::vector<std::string> Filters;
		std::map<ResourceType, FileLoadDelegate> FileLoaders;
		std::string WorkingSpace;
		FileMetaDatabase MetaDatabase;
	
		bool IsValidFilter(std::string& InExtension);
	};

	typedef RefPtr<ResourceSystem> FileSystemPtr;

}