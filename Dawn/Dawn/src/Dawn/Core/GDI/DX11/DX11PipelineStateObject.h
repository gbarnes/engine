#pragma once
#include "Core/GDI/Base/GfxPipelineStateObject.h"
#include "Core/GDI/Base/inc_gfx_types.h"
#include "inc_dx11.h"

namespace Dawn
{
	class DX11PipelineStateObject : public GfxPipelineStateObject
	{
	public:
		DX11PipelineStateObject(const GfxPipelineStateObjectDesc& InDesc, const GfxResId& InId, GfxGDIPtr InGDI);
		~DX11PipelineStateObject();
		virtual void Create() override;

		inline ID3D11BlendState* GetD3D11BlendState() const
		{
			return BlendState.Get();
		}

		inline ID3D11RasterizerState* GetD3D11RasterizerState() const
		{
			return RasterizerState.Get();
		}

		inline ID3D11DepthStencilState* GetD3D11DepthStencilState() const
		{
			return DepthStencilState.Get();
		}

		inline ID3D11InputLayout* GetD3D11InputLayout() const
		{
			return InputLayout.Get();
		}

	private:
		ComPtr<ID3D11BlendState>        BlendState;
		ComPtr<ID3D11RasterizerState>   RasterizerState;
		ComPtr<ID3D11DepthStencilState> DepthStencilState;
		ComPtr<ID3D11InputLayout>       InputLayout;
	};
}