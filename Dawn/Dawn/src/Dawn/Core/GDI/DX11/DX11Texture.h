#pragma once
#include "Core/GDI/Base/GfxTexture.h"
#include "inc_dx11.h"

namespace Dawn
{
	class DX11Texture2D : public GfxTexture
	{
	public:
		DX11Texture2D(const GfxResId& InId, GfxGDIPtr InGDI);
		virtual ~DX11Texture2D();

		void Create(const GfxTextureDesc& InDesc, const GfxTextureData& InData);
		virtual void SetName(const std::string& InName) override;
		virtual GfxResId CreateView(const GfxTextureViewDesc& InDesc, GfxTextureView** OutView) override;
		virtual void* GetGPUAddress() const override;
	private:
		ComPtr<ID3D11Texture2D> Resource;
	};
}