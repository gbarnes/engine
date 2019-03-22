#pragma once
#include "Resources.h"

namespace Dawn
{
	class ResourceSystem;
	struct FileMetaData;

	MeshHandle LoadStaticMesh(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData);

}