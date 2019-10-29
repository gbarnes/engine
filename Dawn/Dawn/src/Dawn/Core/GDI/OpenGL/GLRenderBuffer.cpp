#include "GLRenderBuffer.h"
#include "../GfxGDI.h"
#include "glad.h"

namespace Dawn
{
	GLRenderBuffer::GLRenderBuffer(GfxResId InId)
		: GfxRenderBuffer(InId)
	{
		glGenFramebuffers(1, &FrameBufferId);

		for (u32 i = 0; i < ColorTargets.size(); ++i)
			ColorTargets[i] = 0;
	}

	GLRenderBuffer::~GLRenderBuffer()
	{
		for(u32 ColorTargetId : ColorTargets)
			if(ColorTargetId > 0)
				glDeleteTextures(1, &ColorTargetId);

		glDeleteFramebuffers(1, &FrameBufferId);
	}

	void GLRenderBuffer::AttachColorTarget(u32 InIndex, u32 InWidth, u32 InHeight, GfxTextureFormat InFormat, GfxTextureFormat InChannel,
		GfxMemoryType InMemoryType)
	{
		Bind(false);

		u32 RenderId = 0;
		glDeleteTextures(1, &RenderId);
		glGenTextures(1, &RenderId);
		glBindTexture(GL_TEXTURE_2D, RenderId);
		glTexImage2D(GL_TEXTURE_2D, 0, InFormat, InWidth, InHeight, 0, InChannel, InMemoryType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + InIndex, GL_TEXTURE_2D, RenderId, 0);

		ColorTargets[InIndex] = RenderId;

		Unbind();
	}

	void GLRenderBuffer::AttachDepthStencilTarget(u32 InWidth, u32 InHeight)
	{
		Bind(false);

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

	void GLRenderBuffer::AttachDepthTarget(u32 InWidth, u32 InHeight)
	{
		Bind(false);

		glDeleteTextures(1, &InternalDepthBufferId);
		glGenTextures(1, &InternalDepthBufferId);
		glBindTexture(GL_TEXTURE_2D, InternalDepthBufferId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, InWidth, InHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, InternalDepthBufferId, 0);

		Unbind();
	}

	void GLRenderBuffer::Bind(bool bAttachTargets)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferId);

		if (bAttachTargets)
		{
			u32 buffers = 0;
			std::vector<u32> Attachments;
			for (u32 Buffer : ColorTargets)
			{
				if (Buffer > 0)
				{
					Attachments.push_back(GL_COLOR_ATTACHMENT0 + buffers);
					++buffers;
				}
			}

			if (buffers > 0)
			{
				glDrawBuffers(buffers, Attachments.data());
			}
			else 
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, InternalDepthBufferId, 0);
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
		}
	}

	void GLRenderBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLRenderBuffer::BindColorTarget(u32 InIndex)
	{
		glBindTexture(GL_TEXTURE_2D, ColorTargets[InIndex]);
	}

	void GLRenderBuffer::UnbindColorTarget(u32 InIndex)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GLRenderBuffer::BindDepthBuffer()
	{
		glBindTexture(GL_TEXTURE_2D, InternalDepthBufferId);
	}

	void GLRenderBuffer::UnbindDepthBuffer()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void* GLRenderBuffer::GetColorTarget(u32 InIndex)
	{
		return (void*)(intptr_t)ColorTargets[InIndex];
	}

	void* GLRenderBuffer::GetDepthTarget()
	{
		return (void*)(intptr_t)InternalDepthBufferId;
	}
}