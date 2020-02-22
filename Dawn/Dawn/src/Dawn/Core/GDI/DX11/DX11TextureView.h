#pragma once
#include "Core/GDI/Base/GfxTextureView.h"
#include "inc_dx11.h"

namespace Dawn
{
	class DX11TextureView : public GfxTextureView
	{
	public:
		DX11TextureView(const GfxResId& InId, GfxGDIPtr InGDI);
		virtual ~DX11TextureView();
		virtual void Create(const GfxTextureViewDesc& InDesc) override;
		virtual void SetName(const std::string& InName) override;
		inline ID3D11View* GetD3DView() const { return View.Get(); }
	private:
		ComPtr<ID3D11View> View;
	};
}