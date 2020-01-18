#pragma once

namespace Dawn
{
	struct Mesh;
	struct Image;
	struct FileMetaData;

	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData);
}