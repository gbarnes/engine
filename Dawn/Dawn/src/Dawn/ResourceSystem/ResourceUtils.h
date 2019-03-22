#pragma once
#include "Core/GDI/GfxCmdList.h"

namespace Dawn
{
	struct Mesh;

	void CopyMeshesToGPU(GfxCmdList& InCmdList, Mesh** InMesh, u32 InNum);
}