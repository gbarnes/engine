#pragma once
#include "inc_common.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	typedef GenericHandle MeshHandle;
	typedef GenericHandle MaterialHandle;
	typedef GenericHandle ShaderHandle;
	typedef GenericHandle GfxResourceViewHandle;
	typedef GenericHandle ImageHandle;
	typedef GenericHandle ModelHandle;

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


	struct DAWN_API Mesh
	{
		MeshHandle Id;
		const char* Name;
		GfxResId VertexArrayId;
		u32 NumIndices;
		u32 NumVertices;
		std::vector<MaterialHandle> Materials;
	};

	struct DAWN_API Shader
	{
		ShaderHandle Id;
		FileHandle FileId;
		GfxResId ResourceId;
	};

	struct DAWN_API Material
	{
		FileHandle HandleToFile;
		MaterialHandle Id;
		ShaderHandle ShaderId;

		// todo: make this more generic but for now we will store all the data we need in
		// this material instead of having a more flexible system depending on the material. 

		vec4 DiffuseColor;
		vec4 SpecularColor;
		vec4 AmbientColor;
		float Shinieness;
	};

	struct DAWN_API Model
	{
		ModelHandle Id;
		FileHandle FileId;
		std::vector<MeshHandle> Meshes;
		std::vector<MaterialHandle> Materials;
	};

	struct DAWN_API Image
	{
		ImageHandle Id;
		FileHandle FileId;
		u8* Pixels;
		u32 Width;
		u32 Height;
		u16 ChannelsPerPixel;
		GfxResId TextureId;
	};
}