#include "stdafx.h"
#include "DX11ShaderResourceCache.h"
#include "DX11Buffer.h"
#include "DX11Sampler.h"

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