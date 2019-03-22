#pragma once
#include "Core/GDI/GfxCmdList.h"

namespace Dawn
{
	struct Mesh;
	struct Image;
	struct FileMetaData;

	void CopyImagesToGPU(GfxCmdList& InCmdList, Image** InImage, u32 InNum);
	void CopyMeshesToGPU(GfxCmdList& InCmdList, Mesh** InMesh, u32 InNum);
	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData);
}