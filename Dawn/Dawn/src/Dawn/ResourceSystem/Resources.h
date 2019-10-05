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

		GfxVertexArray* GetGfxResource()
		{
			return GfxGDI::Get()->GetVertexArray(VertexArrayId);
		}
	};

	struct DAWN_API Shader
	{
		ShaderHandle Id;
		FileHandle FileId;
		GfxResId ResourceId;

		GfxShader* GetResource()
		{
			return GfxGDI::Get()->GetShader(ResourceId);
		}
	};

	struct DAWN_API Material
	{
		FileHandle HandleToFile;
		MaterialHandle Id;
		ShaderHandle ShaderId;
	};

	struct DAWN_API Model
	{
		ModelHandle Id;
		FileHandle FileId;
		std::vector<MeshHandle> Meshes;
		std::vector<MaterialHandle> Materials;

		/*std::shared_ptr<Mesh> GetMesh(u32 Index)
		{
			auto id = Meshes[Index];
			return ResourceTable::GetMesh(id);
		}*/

		/*std::shared_ptr<Material> GetMaterial(u32 Index)
		{
			return ResourceTable::GetMaterial(Materials[Index]);
		}*/
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

		GfxTexture* GetResource()
		{
			return GfxGDI::Get()->GetTexture(TextureId);
		}
	};
}