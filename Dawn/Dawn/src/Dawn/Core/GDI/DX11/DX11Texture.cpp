#include "stdafx.h"
#include "DX11Texture.h"
#include "DX11TextureView.h"
#include "DX11GDI.h"
#include "inc_dx11_utils.h"

Dawn::DX11Texture2D::DX11Texture2D(const GfxResId& InId, GfxGDIPtr InGDI)
	: GfxTexture(InId, InGDI)
{
}

Dawn::DX11Texture2D::~DX11Texture2D()
{
	Resource = nullptr;
}

void Dawn::DX11Texture2D::Create(const GfxTextureDesc& InDesc, const GfxTextureData& InData)
{
	D3D11_TEXTURE2D_DESC desc = ToD3DTextureDesc(InDesc);
	DX11GDI* gdi = static_cast<DX11GDI*>(GDI.get());

	if (InData.Data != nullptr)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = InData.Data;
		data.SysMemPitch = InData.ChannelsPerPixel * InDesc.Width;
		data.SysMemSlicePitch = data.SysMemPitch * InDesc.Height;

		HRESULT hr = gdi->GetD3D11Device()->CreateTexture2D(&desc, &data, &Resource);
		D_ASSERT(SUCCEEDED(hr), "Couldn't create texture!");
	}
	else
	{
		HRESULT hr = gdi->GetD3D11Device()->CreateTexture2D(&desc, nullptr, &Resource);
	}
}

void Dawn::DX11Texture2D::SetName(const std::string& InName)
{
	Resource->SetPrivateData(WKPDID_D3DDebugObjectName, InName.size(), InName.c_str());
}

Dawn::GfxResId Dawn::DX11Texture2D::CreateView(const GfxTextureViewDesc& InDesc, GfxTextureView** OutView)
{
	DX11GDI* gdi = static_cast<DX11GDI*>(GDI.get());

	GfxTextureView* view = nullptr;
	GfxResId id = gdi->CreateTextureView(InDesc, &view);
	*OutView = view;
	return id;
}

void* Dawn::DX11Texture2D::GetGPUAddress() const
{
	return static_cast<void*>(Resource.Get());
}