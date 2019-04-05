#pragma once
#include <inc_common.h>
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	struct Mesh;
	struct Image;
	struct FileMetaData;

	void CopyImagesToGPU(Image** InImage, u32 InNum, const GfxWrapDesc& InWrap, const GfxFilterDesc& InFilter, bool InGenerateMipMaps = true);
	void CopyMeshesToGPU(Mesh** InMesh, u32 InNum);
	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData);
}