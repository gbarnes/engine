#include "stdafx.h"
#include "DX11Sampler.h"
#include "DX11GDI.h"
#include "inc_dx11_utils.h"

Dawn::DX11Sampler::DX11Sampler(GfxResId InId, GfxGDIPtr InGDI)
	: GfxSampler(InId, InGDI)
{

}

Dawn::DX11Sampler::~DX11Sampler()
{
	Sampler = nullptr;
}

void Dawn::DX11Sampler::Create(const GfxSamplerDesc& InDesc)
{
	DX11GDI* dxGDI = static_cast<DX11GDI*>(GDI.get());
	auto desc = ToDX11SamplerDesc(InDesc);
	dxGDI->GetD3D11Device()->CreateSamplerState(&desc, &Sampler);
}