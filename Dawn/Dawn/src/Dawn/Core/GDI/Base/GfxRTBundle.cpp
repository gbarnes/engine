#include "stdafx.h"
#include "GfxRTBundle.h"
#include "GfxTextureView.h"
#include "GfxTexture.h"

Dawn::GfxRTBundle::GfxRTBundle()
	: CurrentRTIndex(0)
{

}

Dawn::GfxRTBundle::GfxRTBundle(GfxGDIPtr InGDI)
	: GDI(InGDI), CurrentRTIndex(0)
{
}

Dawn::GfxRTBundle::~GfxRTBundle()
{
	GDI = nullptr;
}

void Dawn::GfxRTBundle::AttachColorTarget(u32 InWidth, u32 InHeight, GfxFormat InFormat)
{
	D_ASSERT(CurrentRTIndex < 7, "More than seven render targets bound!");

	GfxTextureDesc desc = {};
	desc.Width = InWidth;
	desc.Height = InHeight;
	desc.Format = InFormat;
	desc.Usage = GfxUsageFlags::Usage_Default;
	desc.CpuAccess = GfxCpuAccessFlags::CpuAccess_Read;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.BindFlags = GfxBindFlags::Bind_ShaderResource | GfxBindFlags::Bind_RenderTargetView;
	desc.SampleCount = 1;
	desc.SampleQuality = 0;

	GfxTextureData data = {};
	data.Data = nullptr;
	data.ChannelsPerPixel = 4;
	data.Size = 0;

	GfxTexture* texture = nullptr;
	GDI->CreateTexture(desc, data, &texture);
	D_ASSERT(texture != nullptr, "Couldn't create texture resource for render target!");
	texture->SetName(fmt::format("Texture ColorTarget {0} {1}", InWidth, InHeight));

	GfxTextureViewDesc viewDesc = {};
	viewDesc.AdressToGPU = texture->GetGPUAddress();
	viewDesc.Type = GfxTextureViewType::RenderTargetView;

	GfxTextureView* rtView = nullptr;
	RenderTargets[CurrentRTIndex] = texture->CreateView(viewDesc, &rtView);
	D_ASSERT(rtView != nullptr, "Couldn't create render target view!");
	++CurrentRTIndex;
}

void Dawn::GfxRTBundle::SetDephtStencilTarget(u32 InWidth, u32 InHeight, GfxFormat InFormat)
{
	D_ASSERT(!DepthStencilTarget.IsValid, "A depth stencil target already set!");

	GfxTextureDesc desc = {};
	desc.Width = InWidth;
	desc.Height = InHeight;
	desc.Format = InFormat;
	desc.Usage = GfxUsageFlags::Usage_Default;
	desc.CpuAccess = GfxCpuAccessFlags::CpuAccess_None;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = InFormat;
	desc.BindFlags = GfxBindFlags::Bind_DepthStencilView;
	desc.SampleCount = 1;
	desc.SampleQuality = 0;

	GfxTextureData data = {};
	data.Data = nullptr;
	data.ChannelsPerPixel = 4;
	data.Size = 0;


	GfxTexture* texture = nullptr;
	GDI->CreateTexture(desc, data, &texture);
	D_ASSERT(texture != nullptr, "Couldn't create texture resource for depth stencil buffer!");

	texture->SetName(fmt::format("Texture DepthStencil {0} {1}", InWidth, InHeight));

	GfxTextureViewDesc viewDesc = {};
	viewDesc.AdressToGPU = texture->GetGPUAddress();
	viewDesc.Type = GfxTextureViewType::DepthStencilView;

	GfxTextureView* rtView = nullptr;
	DepthStencilTarget = texture->CreateView(viewDesc, &rtView);
	D_ASSERT(rtView != nullptr, "Couldn't create depth stencil view!");
}

Dawn::GfxTextureView* Dawn::GfxRTBundle::GetColorTarget(i32 InIndex) const
{
	return GDI->GetTextureView(RenderTargets[InIndex]);
}

Dawn::GfxTextureView* Dawn::GfxRTBundle::GetDepthStencilTarget() const
{
	return GDI->GetTextureView(DepthStencilTarget);
}