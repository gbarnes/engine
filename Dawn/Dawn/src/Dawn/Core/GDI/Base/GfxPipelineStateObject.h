#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"
#include "GfxShaderResourceCache.h"

namespace Dawn
{
	class GfxPipelineStateObject : public GfxResource
	{
	public:
		GfxPipelineStateObject(const GfxPipelineStateObjectDesc& InDesc, const GfxResId& InId, GfxGDIPtr InGDI)
			: GfxResource(InId, InGDI), Desc(InDesc) 
		{
			if (Desc.HullShaderId.IsValid) 
			{
				BoundShaderTypes.Push((u8)GfxShaderType::Hull);
			}
			if (Desc.PixelShaderId.IsValid) 
			{
				BoundShaderTypes.Push((u8)GfxShaderType::Pixel);
			}
			if (Desc.VertexShaderId.IsValid) 
			{
				BoundShaderTypes.Push((u8)GfxShaderType::Vertex);
			}
			if (Desc.GeometryShaderId.IsValid) 
			{
				BoundShaderTypes.Push((u8)GfxShaderType::Geometry);
			}
			if (Desc.ComputeShaderId.IsValid) 
			{
				BoundShaderTypes.Push((u8)GfxShaderType::Compute);
			}
		};

		virtual ~GfxPipelineStateObject()
		{
			for (i64 i = 0; i < BoundShaderTypes.Count(); ++i)
			{
				SafeDelete(ResourceCaches[BoundShaderTypes[i]]);
			}
		}

		virtual void Create() = 0;

		inline const GfxPipelineStateObjectDesc& GetDesc() const 
		{
			return Desc;
		}

		inline i32 GetNumActiveShaders() const
		{
			return NumActiveShaders;
		}

		inline GfxShaderResourceCache* GetResourceCache(GfxShaderType InType) const
		{
			return ResourceCaches[u32(InType)];
		}

		inline const Array<u8>& GetBoundShaderTypes() const
		{
			return BoundShaderTypes;
		}

	protected:
		i32 NumActiveShaders;
		GfxPipelineStateObjectDesc Desc;
		Array<u8> BoundShaderTypes;
		GfxShaderResourceCache* ResourceCaches[u8(GfxShaderType::Num)]; // index corresponds to shader number
	};
}