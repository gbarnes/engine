#pragma once
#include "inc_common.h"
#include "Core/GDI/GfxVerticeFormats.h"
#include <d3d12.h>

namespace Dawn
{
	typedef u128 FileHandle;
	typedef GenericHandle MeshHandle;
	typedef GenericHandle MaterialHandle;
	typedef GenericHandle ShaderHandle;
	typedef GenericHandle GfxResourceViewHandle;

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

	struct DAWN_API FileMetaData
	{
		FileHandle Id;
		std::string Name;
		std::string Path;
		std::string Ext;
		ResourceType Type;
		u128 Size;
	};

	struct DAWN_API Submesh
	{
		std::string Name;
		u128 NumOfIndices;
		u32 StartIndex;
	};

	struct DAWN_API Shader
	{
		FileHandle HandleToFile;
		u32 Id;
		ComPtr<ID3DBlob> D3DData;
	};

	struct DAWN_API Material
	{
		FileHandle HandleToFile;
		u32 Id;
	};

	struct DAWN_API Mesh
	{
		MeshHandle Id;
		FileHandle FileId;
		std::vector<VertexPosNormalUV> Vertices;
		std::vector<u16> Indices;
		std::vector<u16> IndicesNormals;
		std::vector<u16> IndicesUV;
		std::vector<Submesh> Submeshes;
		std::vector<MaterialHandle> Materials;
		GfxResourceViewHandle IndexBufferView;
		GfxResourceViewHandle VertexBufferView;
	};

	struct DAWN_API AnimatedMesh : public Mesh
	{
	};

	struct DAWN_API Image
	{
		FileHandle HandleToFile;

		u8* Pixels;
		u32 Width;
		u32 Height;
		u16 ChannelsPerPixel;
	};

	struct DAWN_API ResourceDatabase
	{
		std::vector<Mesh> Meshes;
		std::vector<Material> Materials;
		std::vector<Shader> Shaders;
		std::vector<Image> Images;
	};
}