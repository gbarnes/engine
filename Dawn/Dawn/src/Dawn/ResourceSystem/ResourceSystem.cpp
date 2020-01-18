#include "stdafx.h"
#include "ResourceSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Core/Loading/File.h"
#include "Core/Logging/Log.h"
#include "Core/Paths.h"
#include "ResourceWatcher.h"

namespace fs = std::filesystem;

namespace Dawn
{
	static ResourceType GetResourceTypeFromExtension(std::string InExtension)
	{
		if (InExtension == ".obj")
			return ResourceType_Model;

		if (InExtension == ".fbx")
			return ResourceType_Model;

		if (InExtension == ".jpg" || InExtension == ".png" || InExtension == ".bmp" || InExtension == ".PNG")
			return ResourceType_Image;

		if (InExtension == ".shader")
			return ResourceType_Shader;

		if (InExtension == ".level")
			return ResourceType_Level;

		return ResourceType_None;
	}

	ResourceSystem::ResourceSystem(Path InPath, std::vector<std::string> InFilter, bool InWatchForChanges)
		: WorkingSpace(InPath.generic_string() + "/"), Filters(InFilter), bWatchForChanges(InWatchForChanges)
	{
	}

	ResourceSystem::~ResourceSystem()
	{
	}

	void ResourceSystem::Shutdown()
	{
		ResourceWatcher::StopMonitoring(ResourcesWatchData.get());
		Resources.Meshes.Clear();
		Resources.Models.Clear();
		Resources.Shaders.Clear();
		Resources.Images.Clear();
		Resources.Materials.Clear();
		Resources.Levels.Clear();
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
				MetaDatabase.emplace(std::pair<FileId, FileMetaData>(metaData.Id, metaData));


				DWN_CORE_INFO("Added {0} ({1}) to file database...", metaData.Name, metaData.Id);
			}
		}

		if(bWatchForChanges)
			ResourcesWatchData.reset(ResourceWatcher::StartMonitoring(this->shared_from_this(), Paths::ProjectContentDir()));

		return true;
	}

	void ResourceSystem::Refresh()
	{
		if (!bWatchForChanges)
			return;

		auto SetOfFiles = ResourceWatcher::GrabLatestFileChanges(ResourcesWatchData.get());
		for (auto File : SetOfFiles)
		{
			auto FileHandle = GetMetaDataFromHandle(CREATE_FILE_HANDLE(Path(File).generic_string()));
			if (!FileHandle)
				continue;

			DWN_CORE_INFO("Detected file change: {0}", File);
			auto it = FileLoaders.find(FileHandle->Ext);
			if (it == FileLoaders.end())
			{
				DWN_CORE_ERROR("There is no resource loader for extension {0}!", FileHandle->Ext);
				return;
			}

			it->second.OnReloadDelegate(this, WorkingSpace, FileHandle);
		}
	}

	void ResourceSystem::RegisterLoader(const FileLoadDelegate& InLoadDelegate, const FileLoadDelegate& InReloadDelegate, const std::list<std::string> & InExtensions)
	{
		for (const std::string& Extension : InExtensions)
		{
			if (FileLoaders.find(Extension) != FileLoaders.end())
			{
				DWN_CORE_ERROR("Loader for extension {0} already registered.", Extension);
				continue;
			}

			FileLoaderDelegates Delegates = {};
			Delegates.OnLoadDelegate = InLoadDelegate;
			Delegates.OnReloadDelegate = InReloadDelegate;

			FileLoaders.emplace(std::make_pair(Extension, Delegates));
		}
	}


	FileMetaData* ResourceSystem::GetMetaDataFromHandle(FileId InHandle)
	{
		auto It = MetaDatabase.find(InHandle);
		if (It == MetaDatabase.end())
			return nullptr;

		return &It->second;
	}

	FileId ResourceSystem::GetFileId(const std::string& InFilename)
	{
		return CREATE_FILE_HANDLE(InFilename);
	}

	ResourceId ResourceSystem::LoadFile(const std::string& InFilename)
	{
		return LoadFile(GetFileId(InFilename));
	}

	ResourceId ResourceSystem::LoadFile(FileId InHandle)
	{
		auto StartTime = std::chrono::steady_clock::now();

		FileMetaData* meta = GetMetaDataFromHandle(InHandle);
		if (meta == nullptr) 
		{
			DWN_CORE_ERROR("File with handle {0} couldn't be found.", InHandle);
			return INVALID_HANDLE;
		}

		if (meta->Name.empty())
		{
			DWN_CORE_ERROR("File with handle {0} couldn't be found.", InHandle);
			return INVALID_HANDLE;
		}

		auto it = FileLoaders.find(meta->Ext);
		if (it == FileLoaders.end())
		{
			DWN_CORE_ERROR("There is no resource loader for extension {0}!", meta->Ext);
			return INVALID_HANDLE;
		}

		ResourceId Id = it->second.OnLoadDelegate(this, WorkingSpace, meta);

		if(FileIdToResourceId.find(InHandle) == FileIdToResourceId.end())
			this->FileIdToResourceId.emplace(std::pair<FileId, ResourceId>(InHandle, Id));

		auto EndTime = std::chrono::steady_clock::now();
		auto TimeInSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);

		DWN_CORE_INFO("Resource {0} loaded into memory in {1} ms.", meta->Name, TimeInSeconds.count());
		
		return Id;
	}

	bool ResourceSystem::IsValidFilter(std::string& InExtension)
	{
		return std::find(Filters.begin(), Filters.end(), InExtension) != Filters.end();
	}
	
	ResourceId ResourceSystem::FindResourceIdFromFileId(FileId InFileId)
	{
		if (this->FileIdToResourceId.find(InFileId) == FileIdToResourceId.end())
			return INVALID_HANDLE;

		return FileIdToResourceId[InFileId];
	}

	std::vector<FileMetaData> ResourceSystem::GetAllMetaFiles()
	{
		std::vector<FileMetaData> files;
		for (auto it : this->MetaDatabase) 
		{
			files.emplace_back(it.second);
		}

		return files;
	}
}