#pragma once
#include "inc_common.h"
#include "../GfxResource.h"

namespace Dawn
{
	class GLTexture : public GfxTexture
	{
	public:
		GLTexture(GfxResId Id, const GfxTextureDesc& InDesc);

		~GLTexture();

		void Bind() override;
		void Unbind() override;
		void Reset(const GfxTextureDesc& InDesc) override;
	private:
		u32 RendererId;
		GfxWrapDesc Wrap;
		GfxFilterDesc Filter;
	};
}