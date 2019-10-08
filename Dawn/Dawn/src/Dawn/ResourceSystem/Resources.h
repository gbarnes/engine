#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	typedef GenericHandle ResourceId;


	enum ResourceType
	{
		ResourceType_None,
		ResourceType_Model,
		ResourceType_Shader,
		ResourceType_Material,
		ResourceType_StaticMesh,
		ResourceType_AnimatedMesh,
		ResourceType_EntityDefinition,
		ResourceType_Image,
		ResourceType_Count
	};


	struct DAWN_API Resource
	{
		ResourceId Id = INVALID_HANDLE;
		FileId FileId = -1;
		ResourceType Type = ResourceType_None;
	};

	struct DAWN_API Mesh : Resource
	{
		const char* Name;
		GfxResId VertexArrayId;
		u32 NumIndices;
		u32 NumVertices;
		std::vector<ResourceId> Materials;
	};

	struct DAWN_API Shader : Resource
	{
		GfxResId ResourceId;
	};

	struct DAWN_API Material : Resource
	{
		// todo: make this more generic but for now we will store all the data we need in
		// this material instead of having a more flexible system depending on the material. 
		vec4 DiffuseColor;
		vec4 SpecularColor;
		vec4 AmbientColor;
		float Shinieness;

		GfxResId DiffuseTexture = INVALID_HANDLE;
	};

	struct DAWN_API Model : Resource
	{
		std::vector<ResourceId> Meshes;
		std::vector<ResourceId> Materials;
	};

	struct DAWN_API Image : Resource
	{
		u32 Width;
		u32 Height;
		u16 ChannelsPerPixel;
		GfxResId TextureId;
	};
}