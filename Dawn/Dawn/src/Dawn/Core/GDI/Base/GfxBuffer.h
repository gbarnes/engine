#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxBuffer : public GfxResource
	{
	public:
		GfxBuffer(GfxResId InId, GfxGDIPtr InGDI)
			: GfxResource(InId, InGDI) {};

		virtual void Create(const GfxBufferDesc& InDesc, const GfxBufferData& InData) = 0;
		virtual u32 GetStride() const = 0;
	};
}