#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxSampler : public GfxResource
	{
	public:
		GfxSampler(const GfxResId& InId, GfxGDIPtr InGDI)
			: GfxResource(InId, InGDI) {};

		virtual void Create(const GfxSamplerDesc& InDesc) = 0;
	};
}