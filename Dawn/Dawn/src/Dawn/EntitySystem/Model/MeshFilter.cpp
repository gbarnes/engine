#include "stdafx.h"
#include "MeshFilter.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/Resources.h"
#include "Application.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(MeshFilter)
		ADD_MEMBER(MeshFilter, Layer, I32Type, true)
		ADD_MEMBER(MeshFilter, ModelFileId, U64Type, true)
		ADD_MEMBER(MeshFilter, MeshIndex, U32Type, true)
	MAKE_TYPE_END()

	Mesh* MeshFilter::GetMesh(ResourceSystem* InResourceSystem)
	{
		if (!MeshId.IsValid)
			return nullptr;

		return InResourceSystem->FindMesh(MeshId);
	}

	void MeshFilter::InitFromLoad(World* InWorld, void* Component)
	{
		MeshFilter* meshFilter = static_cast<MeshFilter*>(Component);
		D_ASSERT(meshFilter != nullptr, "No valid instance of a mesh filter!");

		ResourceSystem* rs = g_Application->GetResourceSystem().get();

		ResourceId modelId = rs->LoadFile(meshFilter->ModelFileId);
		if (modelId.IsValid)
		{
			Model* model = rs->FindModel(modelId);
			meshFilter->MeshId = model->Meshes[meshFilter->MeshIndex];
		}
	}
}