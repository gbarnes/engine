#include "stdafx.h"
#include "DX11TextureView.h"
#include "DX11Texture.h"
#include "DX11GDI.h"
#include "inc_dx11_utils.h"

Dawn::DX11TextureView::DX11TextureView(const GfxResId& InId, GfxGDIPtr InGDI)
	: GfxTextureView(InId, InGDI)
{

}

Dawn::DX11TextureView::~DX11TextureView()
{
	View = nullptr;
}

void Dawn::DX11TextureView::SetName(const std::string& InName)
{
	D_ASSERT(View != nullptr, "Seems as if view is already null");
	View->SetPrivateData(WKPDID_D3DDebugObjectName, InName.size(), InName.c_str());
}

void Dawn::DX11TextureView::Create(const GfxTextureViewDesc& InDesc)
{
	DX11GDI* gdi = static_cast<DX11GDI*>(this->GDI.get());
	ID3D11Device* device = gdi->GetD3D11Device();

	D_ASSERT(InDesc.AdressToGPU != nullptr, "No texture set to create view from!");

	ID3D11Texture2D* texture = static_cast<ID3D11Texture2D*>(InDesc.AdressToGPU);

	if (InDesc.Type == GfxTextureViewType::RenderTargetView)
	{
		ComPtr<ID3D11RenderTargetView> rtView = nullptr;
		HRESULT hr = device->CreateRenderTargetView(texture, nullptr, &rtView);
		D_ASSERT(SUCCEEDED(hr), "Cannot create render target view!");
		View = rtView;
	}
	else if (InDesc.Type == GfxTextureViewType::DepthStencilView)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = ToDX11Format(InDesc.Format);
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		
		ComPtr<ID3D11DepthStencilView> dsView;
		HRESULT hr = device->CreateDepthStencilView(texture, &desc, &dsView);
		D_ASSERT(SUCCEEDED(hr), "Cannot create depth stencil view!");
		View = dsView;
	}
	else if (InDesc.Type == GfxTextureViewType::ShaderResourceView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = ToDX11Format(InDesc.Format);
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = -1;

		ComPtr<ID3D11ShaderResourceView> ssView;
		HRESULT hr = device->CreateShaderResourceView(texture, &desc, &ssView);
		D_ASSERT(SUCCEEDED(hr), "Cannot create shader resource view!");
		View = ssView;
	}
}