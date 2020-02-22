#pragma once
#include "Core/Container/HierarchyGraph.h"
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	typedef GenericHandle ResourceId;


	enum DAWN_API ResourceType
	{
		ResourceType_None,
		ResourceType_Model,
		ResourceType_Shader,
		ResourceType_Material,
		ResourceType_StaticMesh,
		ResourceType_AnimatedMesh,
		ResourceType_EntityDefinition,
		ResourceType_Image,
		ResourceType_Level,
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
		AABBox Bounds;
	};

	struct DAWN_API AnimatedMesh : Resource
	{
		const char* Name;
	};

	struct DAWN_API Shader : Resource
	{
		GfxResId GfxShaderId;
	};

	struct DAWN_API Material : Resource
	{
		// todo: make this more generic but for now we will store all the data we need in
		// this material instead of having a more flexible system depending on the material. 
		std::string Name = "Default Material";
		vec4 Albedo;
		vec4 Emissive;
		float Metallic;
		float Roughness;
		float Ao;
		GfxResId ShaderId;
		GfxResId DiffuseTexture = INVALID_HANDLE;
	};

	struct DAWN_API ModelNodeData
	{
		std::string Name;
		std::vector<i32> Meshes;
		vec3 Position, Scale, Rotation;
	};

	struct DAWN_API Model : Resource
	{
		bool bHasAnimations = false;
		std::vector<ResourceId> Meshes;
		std::vector<ResourceId> Materials;
		HierarchyGraph<ModelNodeData> Hierarchy;
		AABBox Bounds;
	};

	struct DAWN_API Image : Resource
	{
		u32 Width;
		u32 Height;
		u16 ChannelsPerPixel;
		GfxResId TextureId;
	};

	struct SoundEffect : Resource
	{
		
	};

	
	struct ComponentData
	{
		struct ComponentValue
		{
			std::string VariableName;
			std::string Data;
		};

		u32 Id;
		std::string Type;
		std::vector<ComponentValue> ComponentValues;
	};

	struct EntityData
	{
		FileId InFileId; // set only if needed!
		UUID Guid;
		std::string Name;
		UUID ParentEntity;
		std::vector<u32> IdToComponent;
	};

	struct DAWN_API EntityDefinition : Resource
	{
		EntityData Data;
		std::vector<ComponentData> Components;
	};


	struct DAWN_API Level : Resource
	{
		const char* Name;
		std::vector<EntityData> Entities;
		std::vector<ComponentData> Components;
	};
}