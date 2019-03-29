#ifdef USE_DX12_GFX
#include "GfxRenderTarget.h"
#include "inc_gfx.h"

namespace Dawn
{
	GfxRenderTarget::GfxRenderTarget()
		: m_Textures(AttachmentPoint::NumAttachmentPoints)
	{}

	// Attach a texture to the render target.
	// The texture will be copied into the texture array.
	void GfxRenderTarget::AttachTexture(AttachmentPoint attachmentPoint, const GfxTexture& texture)
	{
		m_Textures[attachmentPoint] = texture;
	}

	const GfxTexture& GfxRenderTarget::GetTexture(AttachmentPoint attachmentPoint) const
	{
		return m_Textures[attachmentPoint];
	}

	// Resize all of the textures associated with the render target.
	void GfxRenderTarget::Resize(uint32_t width, uint32_t height)
	{
		for (auto& texture : m_Textures)
		{
			texture.Resize(width, height);
		}
	}

	// Get a list of the textures attached to the render target.
	// This method is primarily used by the CommandList when binding the
	// render target to the output merger stage of the rendering pipeline.
	const std::vector<GfxTexture>& GfxRenderTarget::GetTextures() const
	{
		return m_Textures;
	}

	D3D12_RT_FORMAT_ARRAY GfxRenderTarget::GetRenderTargetFormats() const
	{
		D3D12_RT_FORMAT_ARRAY rtvFormats = {};


		for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i)
		{
			const GfxTexture& texture = m_Textures[i];
			if (texture.IsValid())
			{
				rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture.GetD3D12ResourceDesc().Format;
			}
		}

		return rtvFormats;
	}

	DXGI_FORMAT GfxRenderTarget::GetDepthStencilFormat() const
	{
		DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
		const GfxTexture& depthStencilTexture = m_Textures[AttachmentPoint::DepthStencil];
		if (depthStencilTexture.IsValid())
		{
			dsvFormat = depthStencilTexture.GetD3D12ResourceDesc().Format;
		}

		return dsvFormat;
	}
}
#endif