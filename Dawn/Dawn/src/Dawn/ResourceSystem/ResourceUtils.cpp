#include "stdafx.h"
#include "ResourceUtils.h"
#include "ResourceSystem.h"
#include "Resources.h"


namespace Dawn
{
	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData)
	{
		return (InWorkspacePath + InMetaData->Path + InMetaData->Name);
	}
}