#include "stdafx.h"
#include "inc_dx11_utils.h"
#include "Core/Container/Array.h"

UINT Dawn::ToDX11AccessFlags(GfxCpuAccessFlags InFlags)
{
	UINT flag = 0;
	
	if (InFlags == GfxCpuAccessFlags::CpuAccess_None)
	{
		flag = 0;
		return flag;
	}

	if (IsFlagSet(InFlags, GfxCpuAccessFlags::CpuAccess_Read))
	{
		flag |= D3D11_CPU_ACCESS_READ;
	}
	
	if(IsFlagSet(InFlags, GfxCpuAccessFlags::CpuAccess_Write))
	{
		flag |= D3D11_CPU_ACCESS_WRITE;
	}
	
	return flag;
}

UINT Dawn::ToDX11BindFlags(GfxBindFlags InFlags)
{
	UINT flag = 0;
	if (IsFlagSet(InFlags, GfxBindFlags::Bind_IndexBuffer))
	{
		flag |= D3D11_BIND_INDEX_BUFFER;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_VertexBuffer))
	{
		flag |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_ShaderResource))
	{
		flag |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_ConstantBuffer))
	{
		flag |= D3D11_BIND_CONSTANT_BUFFER;
	}
	
	if (IsFlagSet(InFlags, GfxBindFlags::Bind_StreamOutput))
	{
		flag |= D3D11_BIND_STREAM_OUTPUT;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_UnorderedAccess))
	{
		flag |= D3D11_BIND_UNORDERED_ACCESS;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_RenderTargetView))
	{
		flag |= D3D11_BIND_RENDER_TARGET;
	}

	if (IsFlagSet(InFlags, GfxBindFlags::Bind_DepthStencilView))
	{
		flag |= D3D11_BIND_DEPTH_STENCIL;
	}

	return flag;
}

D3D11_BUFFER_DESC Dawn::ToDX11BufferDesc(const GfxBufferDesc& InDesc)
{
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = ToDX11Usage(InDesc.Usage);
	desc.CPUAccessFlags = ToDX11AccessFlags(InDesc.AccessFlags);
	desc.BindFlags = ToDX11BindFlags(InDesc.BindFlags);
	desc.StructureByteStride = InDesc.StructureByteStride;
	desc.ByteWidth = InDesc.ByteWidth;
	desc.MiscFlags = 0;
	return desc;
}

DXGI_FORMAT Dawn::ToDX11Format(GfxFormat InFormat)
{
	switch (InFormat)
	{
	case GfxFormat::RGBA32F:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case GfxFormat::RGBA16F:
		return DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	case GfxFormat::RGB32F:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case GfxFormat::R16F:
		return DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT;
		break;
	case GfxFormat::RGBA8UN:
		return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case GfxFormat::D24S8:
		return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	}

	return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
}

/*
DXGI_FORMAT Dawn::ToDX11Format(GfxShaderDataType InType)
{
	switch (InType)
	{
	case GfxShaderDataType::Float4:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case GfxShaderDataType::Float3:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
		break;
	case GfxShaderDataType::Float2:
		return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
		break;
	case GfxShaderDataType::Color:
		return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	return DXGI_FORMAT::DXGI_FORMAT_A8_UNORM;
}*/

D3D11_USAGE Dawn::ToDX11Usage(GfxUsageFlags InUsage)
{
	switch (InUsage)
	{
		case GfxUsageFlags::Usage_Default:
			return D3D11_USAGE_DEFAULT;
			break;
		case GfxUsageFlags::Usage_Dynamic:
			return D3D11_USAGE_DYNAMIC;
			break;
		case GfxUsageFlags::Usage_Immutable:
			return D3D11_USAGE_IMMUTABLE;
			break;
		case GfxUsageFlags::Usage_Staging:
			return D3D11_USAGE_STAGING;
			break;
	}

	return D3D11_USAGE_DEFAULT;
}


D3D11_TEXTURE2D_DESC Dawn::ToD3DTextureDesc(const GfxTextureDesc& InDesc)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = InDesc.Width;
	desc.Height = InDesc.Height;
	desc.Format = ToDX11Format(InDesc.Format);
	desc.MipLevels = InDesc.MipLevels;
	desc.ArraySize = InDesc.ArraySize;
	desc.SampleDesc.Count = InDesc.SampleCount;
	desc.SampleDesc.Quality = InDesc.SampleQuality;
	desc.Usage = ToDX11Usage(InDesc.Usage);
	desc.BindFlags = ToDX11BindFlags(InDesc.BindFlags);
	desc.CPUAccessFlags = ToDX11AccessFlags(InDesc.CpuAccess);
	return desc;
}

DXGI_FORMAT Dawn::ShaderTypeToDX11Format(GfxShaderDataType InType)
{
	if (InType == Dawn::GfxShaderDataType::Position)
	{
		return DXGI_FORMAT_R32G32B32_FLOAT;
	}

	if (InType == Dawn::GfxShaderDataType::Color)
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	if (InType == Dawn::GfxShaderDataType::Matrix)
	{
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	if (InType == Dawn::GfxShaderDataType::TexCoord0 || InType == Dawn::GfxShaderDataType::TexCoord1)
	{
		return DXGI_FORMAT_R32G32_FLOAT;
	}

	if (InType == Dawn::GfxShaderDataType::Normal)
	{
		return DXGI_FORMAT_R32G32B32_FLOAT;
	}

	return DXGI_FORMAT_R32G32B32_FLOAT;
}

std::string ShaderTypeToDX11SemanticName(Dawn::GfxShaderDataType InType)
{
	if (InType == Dawn::GfxShaderDataType::Position)
	{
		return "POSITION";
	}

	if (InType == Dawn::GfxShaderDataType::Color)
	{
		return "COLOR";
	}

	if (InType == Dawn::GfxShaderDataType::Matrix)
	{
		return "MATRIX";
	}

	if (InType == Dawn::GfxShaderDataType::TexCoord0)
	{
		return "TEXCOORD";
	}

	if (InType == Dawn::GfxShaderDataType::TexCoord1)
	{
		return "TEXCOORD1";
	}

	if (InType == Dawn::GfxShaderDataType::Normal)
	{
		return "NORMAL";
	}

	return "FLOAT";
}

std::vector<D3D11_INPUT_ELEMENT_DESC> Dawn::ToDX11Layout(const GfxInputLayout& InLayout)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> descEntries;
	
	for (auto it = InLayout.Begin(); it != InLayout.End(); ++it)
	{
		if (it->Type == GfxShaderDataType::Matrix)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (it->IsPerInstance)
				{
					D3D11_INPUT_ELEMENT_DESC desc = { it->SemanticName.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
					descEntries.push_back(desc);
				}
				else
				{
					D3D11_INPUT_ELEMENT_DESC desc = { it->SemanticName.c_str(), i, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
					descEntries.push_back(desc);
				}
			}
		}
		else
		{
			
			D3D11_INPUT_ELEMENT_DESC desc = {};
			
			std::string s = ShaderTypeToDX11SemanticName(it->Type);
			char* ws = new char[s.size() + 1]; // +1 for zero at the end
			copy(s.begin(), s.end(), ws);
			ws[s.size()] = 0; // zero at the end

			desc.SemanticName = ws;
			desc.SemanticIndex = 0;
			desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			desc.Format = ShaderTypeToDX11Format(it->Type);
			desc.InstanceDataStepRate = 0;
			desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			descEntries.push_back(desc);
		}
	}

	return descEntries;
}

D3D11_DEPTH_STENCIL_DESC Dawn::ToDX11DepthStencilDesc(const GfxDepthStencilDesc& InDesc)
{
	D3D11_DEPTH_STENCIL_DESC desc = {};
	memcpy(&desc, &InDesc, sizeof(GfxDepthStencilDesc));
	return desc;
}

D3D11_RASTERIZER_DESC Dawn::ToDX11RasterizerDesc(const GfxRasterizerDesc& InDesc)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = InDesc.AntialiasedLineEnable;
	desc.CullMode = (D3D11_CULL_MODE)InDesc.CullMode;
	desc.FillMode = (D3D11_FILL_MODE)InDesc.FillMode;
	desc.DepthBias = InDesc.DepthBias;
	desc.DepthBiasClamp = InDesc.DepthBiasClamp;
	desc.DepthClipEnable = InDesc.DepthClipEnable;
	desc.FrontCounterClockwise = InDesc.FrontCounterClockwise;
	desc.MultisampleEnable = InDesc.MultisampleEnable;
	desc.ScissorEnable = 0;
	desc.SlopeScaledDepthBias = InDesc.SlopeScaledDepthBias;
	//memcpy(&desc, &InDesc, sizeof(GfxRasterizerDesc));
	return desc;
}

D3D11_BLEND_DESC Dawn::ToDX11BlendDesc(const GfxBlendDesc& InDesc)
{
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = InDesc.AlphaToCoverageEnable;
	desc.IndependentBlendEnable = InDesc.IndependentBlendEnable;
	
	for (int i = 0; i < 8; ++i)
	{
		desc.RenderTarget[i].BlendEnable = InDesc.RenderTarget[i].BlendEnable;
		desc.RenderTarget[i].BlendOp = (D3D11_BLEND_OP)InDesc.RenderTarget[i].BlendOp;
		desc.RenderTarget[i].BlendOpAlpha = (D3D11_BLEND_OP)InDesc.RenderTarget[i].BlendOpAlpha;
		desc.RenderTarget[i].DestBlend = (D3D11_BLEND)InDesc.RenderTarget[i].DestBlend;
		desc.RenderTarget[i].DestBlendAlpha = (D3D11_BLEND)InDesc.RenderTarget[i].DestBlendAlpha;
		desc.RenderTarget[i].RenderTargetWriteMask = (D3D11_BLEND)InDesc.RenderTarget[i].RenderTargetWriteMask;
		desc.RenderTarget[i].SrcBlend = (D3D11_BLEND)InDesc.RenderTarget[i].SrcBlend;
		desc.RenderTarget[i].SrcBlendAlpha = (D3D11_BLEND)InDesc.RenderTarget[i].SrcBlendAlpha;
	}

	return desc;
}

D3D11_SAMPLER_DESC Dawn::ToDX11SamplerDesc(const GfxSamplerDesc& InDesc)
{
	D3D11_SAMPLER_DESC desc = {};
	memcpy(&desc, &InDesc, sizeof(GfxSamplerDesc));
	return desc;
}