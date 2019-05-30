#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx_types.h"

namespace Dawn
{
	typedef GenericHandle MeshHandle;
	typedef GenericHandle MaterialHandle;
	typedef GenericHandle ShaderHandle;
	typedef GenericHandle GfxResourceViewHandle;
	typedef GenericHandle ImageHandle;

	enum ResourceType
	{
		ResourceType_None,
		ResourceType_Shader,
		ResourceType_Material,
		ResourceType_StaticMesh,
		ResourceType_AnimatedMesh,
		ResourceType_EntityDefinition,
		ResourceType_Image,
		ResourceType_Count
	};

	struct DAWN_API Submesh
	{
		std::string Name;
		u128 NumOfIndices;
		u32 StartIndex;
	};

	struct DAWN_API Shader
	{
		ShaderHandle Id;
		FileHandle FileId;
		u32 GDI_ShaderId;
	};

	struct DAWN_API Material
	{
		FileHandle HandleToFile;
		MaterialHandle Id;
		ShaderHandle ShaderId;
	};

	struct DAWN_API Mesh
	{
		MeshHandle Id;
		FileHandle FileId;
		std::vector<VertexPosUV> Vertices;
		std::vector<u16> Indices;
		std::vector<Submesh> Submeshes;
		std::vector<MaterialHandle> Materials;
		u32 NumIndices;
		u32 NumVertices;
		u32 GDI_VBOId, GDI_VAOId, GDI_EBOId; // Refactor this later on!
	};

	struct DAWN_API AnimatedMesh : public Mesh
	{
	};

	struct DAWN_API Image
	{
		ImageHandle Id;
		FileHandle FileId;
		u8* Pixels;
		u32 Width;
		u32 Height;
		u16 ChannelsPerPixel;
		u32 GDI_TextureId;
	};
}