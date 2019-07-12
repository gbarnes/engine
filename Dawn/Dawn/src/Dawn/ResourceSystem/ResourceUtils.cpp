#include "ResourceUtils.h"
#include "ResourceSystem.h"
#include "Resources.h"
#include "Core/GDI/GfxResource.h"
#include <glad.h>


namespace Dawn
{
	void CopyMeshesToGPU(Mesh** InMesh, u32 InNum )
	{
		GfxBufferLayout layout = {
			{GfxShaderDataType::Float4, "aPos"},
			{GfxShaderDataType::Float2, "aUV0"},
			{GfxShaderDataType::Float2, "aUV1"}
		};

		

		for (u32 i = 0; i < InNum; ++i)
		{
			Mesh* mesh = InMesh[i];

			if (mesh->NumIndices > 0 && mesh->NumVertices > 0)
			{
				u128 sizeOfVertex = sizeof(VertexPosUV);

				glGenVertexArrays(1, &mesh->GDI_VAOId);
				glGenBuffers(1, &mesh->GDI_VBOId);
				glGenBuffers(1, &mesh->GDI_EBOId);

				glBindVertexArray(mesh->GDI_VAOId);
				glBindBuffer(GL_ARRAY_BUFFER, mesh->GDI_VBOId);
				glBufferData(GL_ARRAY_BUFFER, mesh->NumVertices * sizeOfVertex, &mesh->Vertices[0], GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GDI_EBOId);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->Indices.size() * sizeof(u16),
					&mesh->Indices[0], GL_STATIC_DRAW);

				// vertex positions
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)0);
				// vertex texture coords0
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)offsetof(VertexPosUV, UV0));
				// vertex texture coords1
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeOfVertex, (void*)offsetof(VertexPosUV, UV1));

				glBindVertexArray(0);
			}
		}
	}

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