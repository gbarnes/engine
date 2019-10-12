#pragma once
#include "inc_common.h"
#include "../GfxResource.h"

namespace Dawn
{
	class GLTexture : public GfxTexture
	{
	public:
		GLTexture(GfxResId Id, u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
			GfxFilterDesc Filter, bool GenerateMipMaps);

		~GLTexture();

		void Bind() override;
		void Unbind() override;
		void Reset(u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
			GfxFilterDesc Filter, bool GenerateMipMaps) override;
	private:
		u32 RendererId;
		GfxWrapDesc Wrap;
		GfxFilterDesc Filter;
	};
}