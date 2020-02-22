#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxTexture;

	class GfxTextureView : public GfxResource
	{
	public:
		GfxTextureView(const GfxResId& InResId, GfxGDIPtr InGDI)
			: GfxResource(InResId, InGDI){}
		virtual void Create(const GfxTextureViewDesc& InDesc) = 0;
	};
}