#pragma once
#include "../GfxResource.h"

namespace Dawn
{
	class GLRenderBuffer : public GfxRenderBuffer
	{
	public:
		GLRenderBuffer(GfxResId InId);
		virtual ~GLRenderBuffer() override;

		void Bind(bool bAttachTargets = true) override;
		void Unbind() override;
		void AttachColorTarget(u32 InIndex, u32 InWidth, u32 InHeight, GfxTextureFormat InFormat = GfxTextureFormat::RGBA16F, 
			GfxTextureFormat InChannel = GfxTextureFormat::RGBA, GfxMemoryType InMemoryType = GfxMemoryType::Float) override;
		void AttachDepthStencilTarget(u32 InWidth, u32 InHeight) override;
		void BindColorTarget(u32 InIndex) override;
		void UnbindColorTarget(u32 InIndex) override;

		void BindDepthBuffer() override;
		void UnbindDepthBuffer() override;
	private:
		u32 FrameBufferId;
		u32 InternalTextureId;
		u32 InternalDepthBufferId;
		std::array<u32, 7> ColorTargets;
	};

}
