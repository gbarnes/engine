#pragma once
#include "Core/GDI/Base/inc_gfx_types.h"
#include "inc_dx11.h"

namespace Dawn
{
	extern std::vector<D3D11_INPUT_ELEMENT_DESC> ToDX11Layout(const GfxInputLayout& InLayout);
	extern D3D11_BUFFER_DESC ToDX11BufferDesc(const GfxBufferDesc& InDesc);
	extern D3D11_USAGE ToDX11Usage(GfxUsageFlags InUsage);
	extern DXGI_FORMAT ToDX11Format(GfxFormat InFormat);
	extern UINT ToDX11AccessFlags(GfxCpuAccessFlags InFlags);
	extern UINT ToDX11BindFlags(GfxBindFlags InFlags);
	extern D3D11_TEXTURE2D_DESC ToD3DTextureDesc(const GfxTextureDesc& InDesc);
	extern D3D11_DEPTH_STENCIL_DESC ToDX11DepthStencilDesc(const GfxDepthStencilDesc& InDesc);
	extern D3D11_RASTERIZER_DESC ToDX11RasterizerDesc(const GfxRasterizerDesc& InDesc);
	extern D3D11_BLEND_DESC ToDX11BlendDesc(const GfxBlendDesc& InDesc);
	extern D3D11_SAMPLER_DESC ToDX11SamplerDesc(const GfxSamplerDesc& InDesc);
	extern DXGI_FORMAT ShaderTypeToDX11Format(GfxShaderDataType InType);
	//DXGI_FORMAT ToDX11Format(GfxShaderDataType InType);
}