#pragma once
#include "Resources.h"

namespace Dawn
{
	class ResourceSystem;
	struct FileMetaData;

	ResourceId RS_LoadModel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadShader(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadImage(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadScene(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadEntityDefinition(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);

	ResourceId RS_ReloadModel(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_ReloadImage(ResourceSystem* InResourceSystem, const std::string& InWorkspacePath, FileMetaData* InMetaData);
}