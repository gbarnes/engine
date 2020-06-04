#pragma once
#include "Core/GDI/Base/GfxShaderResourceCache.h"

struct ID3D11Buffer;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace Dawn
{
	class DX11ShaderResourceCache : public GfxShaderResourceCache
	{
	public:
		virtual void BindConstantBuffer(const GfxBuffer* InBuffer) override;
		virtual void BindSampler(const GfxSampler* InSampler) override;
		virtual void BindShaderResourceView(const GfxTextureView* InView) override;

		ID3D11Buffer** ToConstantBuffers()
		{
			return ConstantBuffers.Data();
		}

		ID3D11SamplerState** ToSamplers()
		{
			return Samplers.Data();
		}

		ID3D11ShaderResourceView** ToShaderResourceViews()
		{
			return ShaderResourceViews.Data();
		}


		i64 NumConstantBuffers() const
		{
			return ConstantBuffers.Count();
		}

		i64 NumSamplers() const
		{
			return Samplers.Count();
		}

		i64 NumShaderResourceViews() const
		{
			return ShaderResourceViews.Count();
		}

		virtual void ClearShaderResourceViews() override
		{
			ShaderResourceViews.Clear();
		}
	private:
		Array<ID3D11Buffer*> ConstantBuffers;
		Array<ID3D11SamplerState*> Samplers;
		Array<ID3D11ShaderResourceView*> ShaderResourceViews;
	};
}