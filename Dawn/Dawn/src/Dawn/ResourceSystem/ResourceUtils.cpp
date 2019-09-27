#include "ResourceUtils.h"
#include "ResourceSystem.h"
#include "Resources.h"
#include "Core/GDI/GfxResource.h"
#include <glad.h>


namespace Dawn
{
	void CopyImagesToGPU(Image** InImage, u32 InNum, const GfxWrapDesc& InWrap, const GfxFilterDesc& InFilter, bool InGenerateMipMaps)
	{
		for (u32 i = 0; i < InNum; ++i)
		{
			Image* image = InImage[i];
			
			// This needs to be refactored later on and done 
			// via the GDI  - gbarnes 03/29/19
			glGenTextures(1, &image->GDI_TextureId);
			glBindTexture(GL_TEXTURE_2D, image->GDI_TextureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, InWrap.WrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, InWrap.WrapT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InFilter.Min);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InFilter.Mag);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->Width, image->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->Pixels);

			if(InGenerateMipMaps)
				glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData)
	{
		return (InWorkspacePath + InMetaData->Path + InMetaData->Name);
	}
}