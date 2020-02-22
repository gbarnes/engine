#pragma once
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxBuffer;
	class GfxSampler;

	class GfxShaderResourceCache
	{
	public:
		virtual ~GfxShaderResourceCache() {}
		virtual void BindConstantBuffer(const GfxBuffer* InBuffer) = 0;
		virtual void BindSampler(const GfxSampler* InSampler) = 0;
	};

}