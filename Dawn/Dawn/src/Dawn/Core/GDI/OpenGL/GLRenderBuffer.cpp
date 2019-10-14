#include "GLRenderBuffer.h"
#include "../GfxGDI.h"
#include "glad.h"

namespace Dawn
{
	GLRenderBuffer::GLRenderBuffer(GfxResId InId)
		: GfxRenderBuffer(InId)
	{
		glGenFramebuffers(1, &FrameBufferId);
	}

	GLRenderBuffer::~GLRenderBuffer()
	{
		glDeleteTextures(1, &InternalTextureId);
		glDeleteFramebuffers(1, &FrameBufferId);
	}

	void GLRenderBuffer::AttachColorTarget(u32 InIndex, u32 InWidth, u32 InHeight)
	{
		Bind();

		glDeleteTextures(1, &InternalTextureId);
		glGenTextures(1, &InternalTextureId);
		glBindTexture(GL_TEXTURE_2D, InternalTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, InWidth, InHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + InIndex, GL_TEXTURE_2D, InternalTextureId, 0);

		Unbind();
	}

	void GLRenderBuffer::AttachDepthStencilTarget(u32 InWidth, u32 InHeight)
	{
		Bind();

		glDeleteTextures(1, &InternalDepthBufferId);
		glGenTextures(1, &InternalDepthBufferId);
		glBindTexture(GL_TEXTURE_2D, InternalDepthBufferId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, InWidth, InHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, InternalDepthBufferId, 0);

		Unbind();
	}

	void GLRenderBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferId);
	}

	void GLRenderBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLRenderBuffer::BindColorTarget(u32 InIndex)
	{
		glBindTexture(GL_TEXTURE_2D, InternalTextureId);
	}

	void GLRenderBuffer::UnbindColorTarget(u32 InIndex)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}