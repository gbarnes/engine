#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "GfxParallel.h"
#include "../../inc_common.h"
#include "GfxDevice.h"

SCmdListHandle CmdListHandles[GfxParallel::ThreadCount];
CGfxFence* Fences[GfxParallel::ThreadCount];
CGfxQueue* CmdQueue = nullptr;

EResult GfxParallel::AllocateResources(CGfxDevice* GfxDevice)
{
	// Initialize the description of the command queue.
	CGfxQueueDesc CommandQueueDesc;
	ZeroMemory(&CommandQueueDesc, sizeof(CommandQueueDesc));

	// Set up the description of the command queue.
	CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CommandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	CommandQueueDesc.NodeMask = 0;

	// Now create the command queue
	if (GfxBackend::CreateQueue(CommandQueueDesc, *GfxDevice, &CmdQueue) == EResult_ERROR)
	{
		return EResult_ERROR;
	}


	for (int i = 0; i < GfxParallel::ThreadCount; i++)
	{
		CGfxCmdAllocator* Allocator = nullptr;
		if (GfxBackend::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, *GfxDevice, &Allocator) == EResult_ERROR)
		{
			return EResult_ERROR;
		}

		CGfxCmdList* List = nullptr;
		SCmdListHandle Handle = GfxBackend::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, *GfxDevice, &List);
		CmdListHandles[i] = Handle;

		// Obtain the list and close it so we can use it later!
		List->Close();
	}

	return EResult_OK;
}

void GfxParallel::FreeResources()
{
	for (int i = 0; i < GfxParallel::ThreadCount; i++)
	{
		//GfxBackend::(CmdListHandles[i]);

		/*if (CmdAllocators[i])
		{
			CmdAllocators[i]->Release();
			CmdAllocators[i] = nullptr;
		}*/
	}


	if (CmdQueue)
	{
		CmdQueue->Release();
		CmdQueue = nullptr;
	}
}

CGfxQueue* GfxParallel::ObtainQueue()
{
	return CmdQueue;
}