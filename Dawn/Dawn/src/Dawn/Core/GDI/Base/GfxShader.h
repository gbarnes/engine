#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxShader : public GfxResource
	{
	public:
		GfxShader(const GfxResId& InId, GfxGDIPtr InGDI)
			: GfxResource(InId, InGDI) {};

		virtual void Create(const GfxShaderDesc& InDesc, const GfxShaderData& InData) = 0;
	};

}