#include "GLTexture.h"
#include "glad.h"

namespace Dawn
{
	GLTexture::GLTexture(GfxResId Id, const GfxTextureDesc& InDesc)
		: GfxTexture(Id)
	{
		Reset(InDesc);
	}

	void GLTexture::Reset(const GfxTextureDesc& InDesc)
	{
		glDeleteTextures(1, &RendererId);
		glGenTextures(1, &RendererId);
		glBindTexture(GL_TEXTURE_2D, RendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, InDesc.Format, InDesc.Width, InDesc.Height, 0, InDesc.Structure, InDesc.MemoryType, InDesc.Data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, InDesc.Wrap.WrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, InDesc.Wrap.WrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InDesc.Filter.Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InDesc.Filter.Mag);

		if (InDesc.bGenerateMipMaps)
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