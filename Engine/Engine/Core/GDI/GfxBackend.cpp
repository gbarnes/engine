#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"
#include "GfxParallel.h"
#include "GfxBackend.h"


//-------------------------------------------------------------------------
// Creates a new dx12 command queue. 
//-------------------------------------------------------------------------
EResult GfxBackend::CreateQueue(CGfxQueueDesc& InDescription, ID3D12Device* InDevice, CGfxQueue** OutQueue)
{
	HRESULT Result;
	Result = InDevice->CreateCommandQueue(&InDescription, __uuidof(ID3D12CommandQueue), (void**)&(*OutQueue));
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	

	return EResult_OK;
}

//-------------------------------------------------------------------------
// Creates a new dx12 allocator! 
//-------------------------------------------------------------------------
CGfxCmdAllocator* g_GfxCmdAllocators[8];
uint32_t g_GfxCmdAllocatorCount = 0;

EResult GfxBackend::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType, ID3D12Device* InDevice, CGfxCmdAllocator** OutAllocator)
{
	HRESULT Result;
	Result = InDevice->CreateCommandAllocator(InType, __uuidof(CGfxCmdAllocator), (void**)(&g_GfxCmdAllocators[g_GfxCmdAllocatorCount]));
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	*OutAllocator = g_GfxCmdAllocators[g_GfxCmdAllocatorCount];

	g_GfxCmdAllocatorCount++;

	return EResult_OK;
}

CGfxCmdAllocator* GfxBackend::ObtainCmdAllocator(const int index) 
{
	return g_GfxCmdAllocators[index];
}
	
//-------------------------------------------------------------------------
// Creates a new dx12 command list! 
//-------------------------------------------------------------------------
CGfxCmdList* g_GfxCmdLists[8];
uint32_t g_GfxCmdListCount = 0;

SCmdListHandle GfxBackend::CreateCommandList(D3D12_COMMAND_LIST_TYPE InType, CGfxCmdAllocator* InAllocator, ID3D12Device* InDevice, CGfxCmdList** OutList)
{
	assert(g_GfxCmdListCount < 8);

	ThrowIfFailed(InDevice->CreateCommandList(0, InType, InAllocator, nullptr, __uuidof(CGfxCmdList), (void**)(&g_GfxCmdLists[g_GfxCmdListCount])));

	SCmdListHandle Handle;
	Handle.Index = g_GfxCmdListCount;
	Handle.Generation = 0;

	*OutList = g_GfxCmdLists[g_GfxCmdListCount];

	g_GfxCmdListCount++;

	return Handle;
}

TODO(Add fail checks!)
CGfxCmdList* GfxBackend::ObtainCmdList(const SCmdListHandle& InStateHandle)
{
	CGfxCmdList* List = g_GfxCmdLists[InStateHandle.Index];
	assert(List != nullptr);
	return List;
}

CGfxCmdList* GfxBackend::ObtainCmdListByIndex(int index)
{
	CGfxCmdList* List = g_GfxCmdLists[index];
	assert(List != nullptr);
	return List;
}

/*void GfxBackend::DeleteCmdList(SCmdListHandle& InStateHandle)
{
	CGfxCmdList* List = g_GfxCmdLists[InStateHandle.Index];
	if (List != nullptr)
	{
		List->Release();
		List = nullptr;
	}
}*/

//-------------------------------------------------------------------------
// Handles fence creation for dx12
//-------------------------------------------------------------------------
CGfxFence* g_GfxFences[8];
uint32_t g_GfxFenceCount = 0;

SFenceHandle GfxBackend::CreateFence(D3D12_FENCE_FLAGS InFlag, CGfxCmdAllocator* InAllocator, ID3D12Device* InDevice)
{
	assert(g_GfxFenceCount < 7);

	CGfxFence* Fence;
	SFenceHandle Handle;
	ThrowIfFailed(InDevice->CreateFence(0, InFlag, __uuidof(ID3D12Fence), (void**)& g_GfxFences[g_GfxFenceCount]));
	Handle.Index = g_GfxFenceCount;
	Handle.Generation = 0;
	g_GfxFenceCount++;
	return Handle;
}

const CGfxFence* GfxBackend::ObtainFence(SFenceHandle& InStateHandle)
{
	assert(InStateHandle.Index < 8);

	CGfxFence* Fence = g_GfxFences[InStateHandle.Index];
	assert(Fence != nullptr);
	return Fence;
}

//void DeleteFence(SCmdListHandle& InStateHandle);

//-------------------------------------------------------------------------
// State Object handling
//-------------------------------------------------------------------------
CGfxState* g_GfxStates[500];
uint32_t g_GfxStatesCount = 0;

SPipelineStateHandle GfxBackend::CreateStateObject(D3D12_GRAPHICS_PIPELINE_STATE_DESC InDescription, ID3D12Device* InDevice)
{
	SPipelineStateHandle Handle;

	ThrowIfFailed(InDevice->CreateGraphicsPipelineState(&InDescription, IID_PPV_ARGS(&g_GfxStates[g_GfxStatesCount])));

	Handle.Index = g_GfxStatesCount;
	Handle.Generation = 0;
	g_GfxStatesCount++;
	return Handle;
}

CGfxState* GfxBackend::ObtainStateObject(SPipelineStateHandle& InStateHandle)
{
	assert(InStateHandle.Index < 500);

	CGfxState* State = g_GfxStates[InStateHandle.Index];
	assert(State != nullptr);
	return State;
}

//-------------------------------------------------------------------------
// Vertex Buffer Handling
//-------------------------------------------------------------------------
CGfxResource* g_GfxVertexBuffers[4000];
uint32_t g_GfxVertexBufferCount = 0;

SVertexBufferHandle GfxBackend::CreateVertexBuffer(const UINT InBufferSize, ID3D12Device* InDevice)
{
	SVertexBufferHandle Handle;

	D3D12_HEAP_PROPERTIES Properties;
	Properties.Type = D3D12_HEAP_TYPE_UPLOAD;

	ThrowIfFailed(InDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(InBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&g_GfxVertexBuffers[g_GfxVertexBufferCount])
		));

	Handle.Index = g_GfxVertexBufferCount;
	Handle.Generation = 0;
	g_GfxVertexBufferCount++;
	return Handle;
}

CGfxResource* ObtainVertexBuffer(SVertexBufferHandle& InStateHandle)
{
	return 0;
}