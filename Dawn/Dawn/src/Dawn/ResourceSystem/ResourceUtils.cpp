#include "ResourceUtils.h"
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


}