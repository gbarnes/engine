#pragma once
#include "Resources.h"

namespace Dawn
{
	class ResourceSystem;
	MeshHandle LoadStaticMesh(ResourceSystem* InFS, std::string& InWorkspacePath, FileMetaData* InMetaData);

}