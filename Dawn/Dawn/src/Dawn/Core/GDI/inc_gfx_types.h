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
#include "d3dx12.h"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef ID3D12CommandQueue CGfxInternalQueue;
typedef D3D12_COMMAND_QUEUE_DESC CGfxQueueDesc;
typedef ID3D12CommandAllocator CGfxCmdAllocator;
typedef ID3D12GraphicsCommandList2 CGfxCmdList;
typedef ID3D12Fence CGfxFence;
typedef D3D12_RESOURCE_BARRIER SGfxResBarrier;
typedef ID3D12Resource CGfxResource;
typedef ID3D12PipelineState CGfxState;
typedef ID3D12DescriptorHeap CGfxHeapDesciptor;
typedef ID3D12RootSignature CGfxRootSignature;
typedef D3D12_INDEX_BUFFER_VIEW CGfxIndexView;
typedef D3D12_VERTEX_BUFFER_VIEW CGfxVertexView;

/*D3D12_INPUT_ELEMENT_DESC SimpleInputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};*/

typedef SGenericHandle<12, 20> SGfxResourceHandle;
typedef SGenericHandle<12, 20> SVertexBufferHandle;
typedef SGenericHandle<12, 20> SIndexBufferHandle;
typedef SGenericHandle<12, 20> SConstantBufferHandle;
typedef SGenericHandle<12, 20> SPipelineStateHandle;
typedef SGenericHandle<12, 20> SCmdListHandle;
typedef SGenericHandle<12, 20> SFenceHandle;

