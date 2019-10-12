#include "GLTexture.h"
#include "glad.h"

namespace Dawn
{
	GLTexture::GLTexture(GfxResId Id, u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
		GfxFilterDesc Filter, bool GenerateMipMaps)
		: GfxTexture(Id)
	{
		Reset(Data, Width, Height, ChannelsPerPixel, Wrap, Filter, GenerateMipMaps);
	}

	void GLTexture::Reset(u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
		GfxFilterDesc Filter, bool GenerateMipMaps)
	{
		glDeleteTextures(1, &RendererId);
		glGenTextures(1, &RendererId);
		glBindTexture(GL_TEXTURE_2D, RendererId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap.WrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap.WrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter.Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter.Mag);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, Data);

		if (GenerateMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLTexture::~GLTexture()
	{
		glDeleteTextures(1, &RendererId);
	}

	void GLTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, RendererId);
	}

	void GLTexture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}