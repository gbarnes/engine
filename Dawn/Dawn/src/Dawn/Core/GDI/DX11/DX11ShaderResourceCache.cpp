#include "stdafx.h"
#include "DX11ShaderResourceCache.h"
#include "DX11Buffer.h"
#include "DX11Sampler.h"
#include "DX11TextureView.h"

void Dawn::DX11ShaderResourceCache::BindConstantBuffer(const GfxBuffer* InBuffer)
{
	const DX11Buffer* buffer = static_cast<const DX11Buffer*>(InBuffer);
	this->ConstantBuffers.Push(buffer->ToD3DBuffer());
}

void Dawn::DX11ShaderResourceCache::BindSampler(const GfxSampler* InSampler)
{
	const DX11Sampler* sampler = static_cast<const DX11Sampler*>(InSampler);
	this->Samplers.Push(sampler->ToD3D11Sampler());
}

void Dawn::DX11ShaderResourceCache::BindShaderResourceView(const GfxTextureView* InView)
{
	const DX11TextureView* texView = static_cast<const DX11TextureView*>(InView);
	auto* dxView = static_cast<ID3D11ShaderResourceView*>(texView->GetD3DView());
	D_ASSERT(dxView != nullptr, "The provided view is no ShaderResourceView");
	this->ShaderResourceViews.Push(dxView);
}