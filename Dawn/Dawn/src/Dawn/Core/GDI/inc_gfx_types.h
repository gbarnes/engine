#pragma once
#include "inc_common.h"

//-----------------------------------------------------------------------------
// LINKING 
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include "d3dx12.h"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef D3D12_COMMAND_QUEUE_DESC CGfxQueueDesc;

typedef D3D12_RESOURCE_BARRIER SGfxResBarrier;
typedef ID3D12PipelineState CGfxState;
typedef ID3D12RootSignature CGfxRootSignature;
typedef D3D12_INDEX_BUFFER_VIEW CGfxIndexView;
typedef D3D12_VERTEX_BUFFER_VIEW CGfxVertexView;

static const CD3DX12_STATIC_SAMPLER_DESC g_PointWrapSampler = { 0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT };

static const CD3DX12_STATIC_SAMPLER_DESC g_PointClampSampler = { 0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP };

static const CD3DX12_STATIC_SAMPLER_DESC g_LinearWrapSampler = { 0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR };

static const CD3DX12_STATIC_SAMPLER_DESC g_LinearClampSampler = { 0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP };

static const CD3DX12_STATIC_SAMPLER_DESC g_AnisotropicWrapSampler = { 0, D3D12_FILTER_COMPARISON_ANISOTROPIC };

static const CD3DX12_STATIC_SAMPLER_DESC g_AnisotropicClampSampler = { 0, D3D12_FILTER_COMPARISON_ANISOTROPIC,
	D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP };


static const D3D12_ROOT_SIGNATURE_FLAGS g_RootSignaturePSVSFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
	D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

enum class TextureUsage
{
	Albedo,
	Diffuse = Albedo,       // Treat Diffuse and Albedo textures the same.
	Heightmap,
	Depth = Heightmap,      // Treat height and depth textures the same.
	Normalmap,
	RenderTarget,           // Texture is used as a render target.
};


namespace Dawn 
{
	typedef DirectX::XMFLOAT3 Vec3;
	typedef DirectX::XMFLOAT2 Vec2;
	typedef DirectX::XMFLOAT4 Vec4;
}
