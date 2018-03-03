#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include "../inc_helpers.h"
#include "../../inc_common.h"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------
typedef ID3D12CommandQueue CGfxQueue;
typedef D3D12_COMMAND_QUEUE_DESC CGfxQueueDesc;
typedef ID3D12CommandAllocator CGfxCmdAllocator;
typedef ID3D12GraphicsCommandList CGfxCmdList;
typedef ID3D12Fence CGfxFence;
typedef D3D12_RESOURCE_BARRIER SGfxResBarrier;
typedef ID3D12Resource CGfxResource;
typedef ID3D12PipelineState CGfxState;

typedef SGenericHandle<12, 20> SGfxResourceHandle;
typedef SGenericHandle<12, 20> SVertexBufferHandle;
typedef SGenericHandle<12, 20> SIndexBufferHandle;
typedef SGenericHandle<12, 20> SConstantBufferHandle;
typedef SGenericHandle<12, 20> SPipelineStateHandle;
typedef SGenericHandle<12, 20> SCmdListHandle;
typedef SGenericHandle<12, 20> SFenceHandle;

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// A graphics queue which can execute command lists. This is a convenience class
// in order to prevent using the dx12 objects directly. 
//-----------------------------------------------------------------------------
namespace GfxBackend
{
	/*D3D12_INPUT_ELEMENT_DESC StaticMeshInputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};*/

	//-------------------------------------------------------------------------
	// Creates a new dx12 command queue. 
	//-------------------------------------------------------------------------
	EResult CreateQueue(CGfxQueueDesc& InDescription, ID3D12Device* InDevice, CGfxQueue** OutQueue);

	//-------------------------------------------------------------------------
	// Creates a new dx12 allocator! 
	//-------------------------------------------------------------------------
	EResult CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType, ID3D12Device* InDevice, CGfxCmdAllocator** OutAllocator);
	CGfxCmdAllocator* ObtainCmdAllocator(const int index);

	//-------------------------------------------------------------------------
	// Creates a new dx12 command list! 
	//-------------------------------------------------------------------------
	SCmdListHandle CreateCommandList(D3D12_COMMAND_LIST_TYPE InType, CGfxCmdAllocator* InAllocator, ID3D12Device* InDevice, CGfxCmdList** OutList);
	CGfxCmdList* ObtainCmdList(const SCmdListHandle& InStateHandle);
	CGfxCmdList* ObtainCmdListByIndex(int index);
	//void DeleteCmdList(SCmdListHandle& InStateHandle);

	//-------------------------------------------------------------------------
	// DX12 Fence Handling
	//-------------------------------------------------------------------------
	SFenceHandle CreateFence(D3D12_FENCE_FLAGS InFlag, CGfxCmdAllocator* InAllocator, ID3D12Device* InDevice);
	const CGfxFence* ObtainFence(SCmdListHandle& InStateHandle);
	//void DeleteFence(SCmdListHandle& InStateHandle);

	//-------------------------------------------------------------------------
	// State Object handling
	//-------------------------------------------------------------------------
	SPipelineStateHandle CreateStateObject(D3D12_GRAPHICS_PIPELINE_STATE_DESC InDescription, ID3D12Device* InDevice);
	CGfxState* ObtainStateObject(SPipelineStateHandle& InStateHandle);

	//-------------------------------------------------------------------------
	// Vertext Buffer Handling
	//-------------------------------------------------------------------------
	SVertexBufferHandle CreateVertexBuffer(const UINT InBufferSize, ID3D12Device* InDevice);
	CGfxResource* ObtainVertexBuffer(SVertexBufferHandle& InStateHandle);
};