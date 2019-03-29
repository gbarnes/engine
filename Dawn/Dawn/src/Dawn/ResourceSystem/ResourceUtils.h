#pragma once
#include <inc_common.h>

namespace Dawn
{
	struct Mesh;
	struct Image;
	struct FileMetaData;

	void CopyImagesToGPU(Image** InImage, u32 InNum);
	void CopyMeshesToGPU(Mesh** InMesh, u32 InNum);
	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData);
}