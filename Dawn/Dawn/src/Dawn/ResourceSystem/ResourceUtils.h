#pragma once
#include <inc_common.h>
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	struct Mesh;
	struct Image;
	struct FileMetaData;

	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData);
}