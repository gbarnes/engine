#include "ResourceUtils.h"
#include "ResourceSystem.h"
#include "Resources.h"



namespace Dawn
{
	void CopyMeshesToGPU(GfxCmdList& InCmdList, Mesh** InMesh, u32 InNum )
	{
		for (u32 i = 0; i < InNum; ++i)
		{
			Mesh* mesh = InMesh[i];
			InCmdList.CopyIndexBuffer(mesh->IndexBufferView, mesh->Indices);
			InCmdList.CopyVertexBuffer(mesh->VertexBufferView, mesh->Vertices);
		}
	}

	void CopyImagesToGPU(GfxCmdList& InCmdList, Image** InImage, u32 InNum)
	{
		for (u32 i = 0; i < InNum; ++i)
		{
			Image* image = InImage[i];
			// This needs to be addressed later on once we want to 
			// load textures to the gpu! Gavin Barnes, 03/22/19
			InCmdList.LoadTextureView(image->TextureView, image);
		}
	}

	std::string ToFullFilePath(std::string InWorkspacePath, FileMetaData* InMetaData)
	{
		return (InWorkspacePath + InMetaData->Path + InMetaData->Name);
	}
}