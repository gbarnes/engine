#pragma once
#include "Resources.h"

namespace Dawn
{
	class ResourceSystem;
	struct FileMetaData;

	ResourceId RS_LoadModel(ResourceSystem* InResourceSystem, std::string& InWorkspacePath, FileMetaData* InMetaData);
	//MeshHandle RS_LoadStaticMesh(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadShader(ResourceSystem* InResourceSystem, std::string& InWorkspacePath, FileMetaData* InMetaData);
	ResourceId RS_LoadImage(ResourceSystem* InResourceSystem, std::string& InWorkspacePath, FileMetaData* InMetaData);
}