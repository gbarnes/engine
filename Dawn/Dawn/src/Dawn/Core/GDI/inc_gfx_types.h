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

enum class TextureUsage
{
	Albedo,
	Diffuse = Albedo,       // Treat Diffuse and Albedo textures the same.
	Heightmap,
	Depth = Heightmap,      // Treat height and depth textures the same.
	Normalmap,
	RenderTarget,           // Texture is used as a render target.
};

/*D3D12_INPUT_ELEMENT_DESC SimpleInputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};*/

/*
typedef SGenericHandle<12, 20> GfxResourceHandle;
typedef SGenericHandle<12, 20> VertexBufferHandle;
typedef SGenericHandle<12, 20> IndexBufferHandle;
typedef SGenericHandle<12, 20> ConstantBufferHandle;
typedef SGenericHandle<12, 20> PipelineStateHandle;
typedef SGenericHandle<12, 20> CmdListHandle;
typedef SGenericHandle<12, 20> FenceHandle;
*/
