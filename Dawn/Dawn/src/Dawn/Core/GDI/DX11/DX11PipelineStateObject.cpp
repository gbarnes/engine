#include "stdafx.h"
#include "DX11PipelineStateObject.h"
#include "DX11GDI.h"
#include "DX11Shader.h"
#include "DX11ShaderResourceCache.h"
#include "inc_dx11_utils.h"


Dawn::DX11PipelineStateObject::DX11PipelineStateObject(const GfxPipelineStateObjectDesc& InDesc, const GfxResId& InId, GfxGDIPtr InGDI)
	: GfxPipelineStateObject(InDesc, InId, InGDI)
{
}

Dawn::DX11PipelineStateObject::~DX11PipelineStateObject()
{
	InputLayout = nullptr;
	RasterizerState = nullptr;
	BlendState = nullptr;
	DepthStencilState = nullptr;
}

void Dawn::DX11PipelineStateObject::Create()
{
	DX11GDI* dxGDI = static_cast<DX11GDI*>(GDI.get());

	// todo (gb): can these states be cached too?
	auto depthDesc = ToDX11DepthStencilDesc(Desc.DepthStencilState);
	HRESULT hr = dxGDI->GetD3D11Device()->CreateDepthStencilState(&depthDesc, &DepthStencilState);
	D_ASSERT(SUCCEEDED(hr), "Couldn't create depth stencil state");

	auto rasterizerDesc = ToDX11RasterizerDesc(Desc.RasterizerState);
	hr = dxGDI->GetD3D11Device()->CreateRasterizerState(&rasterizerDesc, &RasterizerState);
	D_ASSERT(SUCCEEDED(hr), "Couldn't create rasterizer state");

	auto blendDesc = ToDX11BlendDesc(Desc.BlendState);
	hr = dxGDI->GetD3D11Device()->CreateBlendState(&blendDesc, &BlendState);
	D_ASSERT(SUCCEEDED(hr), "Couldn't create blend state");

	if (Desc.InputLayout.Count() > 0 && Desc.VertexShaderId.IsValid)
	{
		DX11Shader* vs = static_cast<DX11Shader*>(dxGDI->GetShader(Desc.VertexShaderId));
		D_ASSERT(vs != nullptr, "Couldn't find vertex shader to create input layout!");
		ID3D10Blob* blob = vs->GetD3D11Blob();

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputDesc = ToDX11Layout(Desc.InputLayout);
		hr = dxGDI->GetD3D11Device()->CreateInputLayout(inputDesc.data(), inputDesc.size(), blob->GetBufferPointer(), blob->GetBufferSize(), &InputLayout);
		D_ASSERT(SUCCEEDED(hr), "Couldn't create input layout!");
	}

	if (Desc.VertexShaderId.IsValid)
	{
		NumActiveShaders++;
		ResourceCaches[u8(GfxShaderType::Vertex)] = new DX11ShaderResourceCache();
	}
	if (Desc.PixelShaderId.IsValid)
	{
		NumActiveShaders++;
		ResourceCaches[u8(GfxShaderType::Pixel)] = new DX11ShaderResourceCache();
	}
	if (Desc.ComputeShaderId.IsValid)
	{
		NumActiveShaders++;
		ResourceCaches[u8(GfxShaderType::Compute)] = new DX11ShaderResourceCache();
	}
	if (Desc.GeometryShaderId.IsValid)
	{
		NumActiveShaders++;
		ResourceCaches[u8(GfxShaderType::Geometry)] = new DX11ShaderResourceCache();
	}
	if (Desc.HullShaderId.IsValid)
	{
		NumActiveShaders++;
		ResourceCaches[u8(GfxShaderType::Hull)] = new DX11ShaderResourceCache();
	}
}