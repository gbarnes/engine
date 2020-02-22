#pragma once
#include "Core/GDI/Base/GfxShaderResourceCache.h"

struct ID3D11Buffer;
struct ID3D11SamplerState;

namespace Dawn
{
	class DX11ShaderResourceCache : public GfxShaderResourceCache
	{
	public:
		virtual void BindConstantBuffer(const GfxBuffer* InBuffer) override;
		virtual void BindSampler(const GfxSampler* InSampler) override;

		ID3D11Buffer** ToConstantBuffers()
		{
			return ConstantBuffers.Data();
		}

		i64 NumConstantBuffers() const
		{
			return ConstantBuffers.Count();
		}
	private:
		Array<ID3D11Buffer*> ConstantBuffers;
		Array<ID3D11SamplerState*> Samplers;
	};
}