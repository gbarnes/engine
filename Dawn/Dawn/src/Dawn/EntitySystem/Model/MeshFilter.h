#pragma once
#include "Core/Type.h"
#include "ResourceSystem/Resources.h"
#include "../Component.h"
#include "Rendering/inc_renderer_common.h"

namespace Dawn
{
	class ResourceSystem;

	struct DAWN_API MeshFilter : public Component<MeshFilter>
	{
		REGISTER_TYPE(MeshFilter)
		static const u32 Version = 1;

		MeshFilter()
		{}

		RenderLayer Layer = RenderLayer::StaticGeom;
		FileId ModelFileId;
		u32 MeshIndex;

		ResourceId MeshId;

		Mesh* GetMesh(ResourceSystem* InResourceSystem);

		static void InitFromLoad(World* InWorld, void* Component);
	};


	extern MeshFilter* CreateMeshFilter(World* InWorld, const Entity& InEntity, const std::string& InFile);
}