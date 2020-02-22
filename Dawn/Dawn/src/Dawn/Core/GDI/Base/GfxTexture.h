#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"
#include "GfxTextureView.h"

namespace Dawn
{
	
	class GfxTexture : public GfxResource
	{
	public:
		GfxTexture(const GfxResId& InId, GfxGDIPtr InGDI) : 
			GfxResource(InId, InGDI) {};

		virtual void Create(const GfxTextureDesc& InDesc, const GfxTextureData& InData) = 0;
		virtual void* GetGPUAddress() const = 0;
		virtual GfxResId CreateView(const GfxTextureViewDesc& InDesc, GfxTextureView** OutView) = 0;

		inline const GfxResId GetId() const {
			return Id;
		}
	};
}