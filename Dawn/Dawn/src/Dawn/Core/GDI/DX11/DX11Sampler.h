#pragma once
#include "Core/GDI/Base/GfxSampler.h"
#include "Core/GDI/Base/GfxGDI.h"
#include "inc_dx11.h"

namespace Dawn
{
	class DX11Sampler : public GfxSampler
	{
	public:
		DX11Sampler(GfxResId InId, GfxGDIPtr InGDI);
		virtual ~DX11Sampler();
		virtual void Create(const GfxSamplerDesc& InDesc) override;

		inline ID3D11SamplerState* ToD3D11Sampler() const
		{
			return Sampler.Get();
		}

	private:
		ComPtr<ID3D11SamplerState> Sampler;
	};
}