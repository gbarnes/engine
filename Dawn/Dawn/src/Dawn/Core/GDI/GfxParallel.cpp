#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "GfxParallel.h"
#include "../../inc_common.h"

namespace Dawn
{
	//CmdListHandle CmdListHandles[GfxParallel::ThreadCount];
	//CGfxFence* Fences[GfxParallel::ThreadCount];
	//ComPtr<GfxInternalQueue> CmdQueue;

	EResult GfxParallel::AllocateResources(GfxDevice* GfxDevice)
	{
		/*CmdQueue = GfxDevice->CreateQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		for (int i = 0; i < GfxParallel::ThreadCount; i++)
		{
			CGfxCmdAllocator* Allocator = nullptr;
			if (GfxBackend::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, *GfxDevice, &Allocator) == EResult_ERROR)
			{
				return EResult_ERROR;
			}

			GfxInternalCmdList* List = nullptr;
			SCmdListHandle Handle = GfxBackend::CreateCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, Allocator, *GfxDevice, &List);
			CmdListHandles[i] = Handle;

			// Obtain the list and close it so we can use it later!
			List->Close();
		}*/

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


		/*	if (CmdQueue)
			{
				CmdQueue->Release();
				CmdQueue = nullptr;
			}*/
	}

	ID3D12CommandQueue* GfxParallel::ObtainQueue()
	{
		return nullptr;//CmdQueue.Get();
	}
}