#pragma once
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxBuffer;
	class GfxSampler;
	class GfxTextureView;

	class GfxShaderResourceCache
	{
	public:
		virtual ~GfxShaderResourceCache() {}
		virtual void BindConstantBuffer(const GfxBuffer* InBuffer) = 0;
		virtual void BindSampler(const GfxSampler* InSampler) = 0;
		virtual void BindShaderResourceView(const GfxTextureView* InView) = 0;
		virtual void ClearShaderResourceViews() = 0;
	};

}