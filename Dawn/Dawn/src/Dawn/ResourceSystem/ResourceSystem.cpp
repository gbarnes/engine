#include "ResourceSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem>
#include "Core/Loading/File.h"
#include "inc_core.h"

namespace fs = std::filesystem;

namespace Dawn
{
	static ResourceType GetResourceTypeFromExtension(std::string InExtension)
	{
		
		if (InExtension == ".obj")
			return ResourceType_StaticMesh;

		if (InExtension == ".fbx")
			return ResourceType_AnimatedMesh;

		if (InExtension == ".jpg" || InExtension == ".png" || InExtension == ".bmp")
			return ResourceType_Image;

		if (InExtension == ".shader")
			return ResourceType_Shader;

		return ResourceType_None;
	}

	ResourceSystem::ResourceSystem()
		: WorkingSpace("")
	{
		Locator::Add(ResourceSystemId, this);
	}

	ResourceSystem::~ResourceSystem()
	{
		Locator::Remove(ResourceSystemId);
	}

	void ResourceSystem::Shutdown()
	{
		ResourceTable::Shutdown();
	}

	bool ResourceSystem::Initialize(std::string InPath, std::vector<std::string> InFilter)
	{
		this->WorkingSpace = InPath;
		this->Filters = InFilter;
		return true;
	}

	bool ResourceSystem::BuildDatabase()
	{
		std::error_code error;
		DWN_CORE_INFO("ResourceSystem start building database...");

		fs::recursive_directory_iterator it = fs::recursive_directory_iterator(this->WorkingSpace, error);

		for (const auto& entry : it)
		{
			std::string extension = entry.path().extension().string();
			if (entry.is_regular_file() && IsValidFilter(extension))
			{
				auto file = entry.path();

				// Implement some sort of meta file creation later on
				// lets just get this up and running for now till we 
				// have some sort of scene format. - Gavin Barnes, 03/21/19

				FileMetaData metaData = {};
				metaData.Name = file.filename().string();

				// strip the workspace path from thus we can combine it and 
				// use it for id generation
				std::string path = file.parent_path().string() + "/";

				std::replace(path.begin(), path.end(), '\\', '/');
				path.erase(path.begin(), path.begin() + WorkingSpace.length());

				metaData.Id = CREATE_FILE_HANDLE(path + metaData.Name);
				metaData.Path = path;
				metaData.Size = entry.file_size();
				metaData.Ext = file.extension().string();
				metaData.Type = GetResourceTypeFromExtension(metaData.Ext);
				MetaDatabase.emplace(std::pair<FileHandle, FileMetaData>(metaData.Id, metaData));

				DWN_CORE_INFO("Added {0} ({1}) to file database...", metaData.Name, metaData.Id);
			}
		}

		return true;
	}

	void ResourceSystem::RegisterLoader(ResourceType InType, FileLoadDelegate InDelegate)
	{
		if (FileLoaders.find(InType) != FileLoaders.end())
		{
			DWN_CORE_ERROR("Loader with type {0} already registered.", InType);
			return;
		}

		FileLoaders.emplace(std::pair<ResourceType, FileLoadDelegate>(InType, InDelegate));
	}

	FileMetaData* ResourceSystem::GetMetaDataFromHandle(FileHandle InHandle)
	{
		if (MetaDatabase.find(InHandle) == MetaDatabase.end()) {
			return nullptr;
		}

		return &MetaDatabase[InHandle];
	}

	GenericHandle ResourceSystem::LoadFile(std::string InFilename)
	{
		return LoadFile(CREATE_FILE_HANDLE(InFilename));
	}

	GenericHandle ResourceSystem::LoadFile(FileHandle InHandle)
	{
		FileMetaData* meta = GetMetaDataFromHandle(InHandle);
		if (meta == nullptr) 
		{
			DWN_CORE_ERROR("File with handle {0} couldn't be found.", InHandle);
			return GenericHandle();
		}

		if (meta->Type == ResourceType_None)
		{
			DWN_CORE_ERROR("File-Type extension {0} not supported!", meta->Ext);
			return GenericHandle();
		}

		auto it = FileLoaders.find(meta->Type);
		if (it == FileLoaders.end())
		{
			DWN_CORE_ERROR("There is no resource loader for type {0}!", meta->Type);
			return GenericHandle();
		}

		GenericHandle file = it->second(this, WorkingSpace, meta);
		return file;
	}

	bool ResourceSystem::IsValidFilter(std::string& InExtension)
	{
		return std::find(Filters.begin(), Filters.end(), InExtension) != Filters.end();
	}

	
}