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

			if (mesh->NumIndices > 0 && mesh->NumVertices > 0)
			{
				size_t sizeOfStruct = sizeof(VertexPosColor);
				DXGI_FORMAT indexFormat = (sizeof(mesh->Indices[0]) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
				InCmdList.CopyIndexBuffer(mesh->IndexBufferView, mesh->NumIndices, indexFormat, mesh->Indices.data());
				InCmdList.CopyVertexBuffer(mesh->VertexBufferView, mesh->NumVertices, sizeof(mesh->Vertices[0]), mesh->Vertices.data());
			}
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