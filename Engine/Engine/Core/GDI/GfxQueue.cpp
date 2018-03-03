//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "GfxQueue.h"

EResult CGfxQueue::Create(D3D12_COMMAND_QUEUE_DESC& InDescription, ID3D12Device* InDevice, CGfxQueue** OutQueue)
{
	HRESULT Result; 
	*OutQueue = new CGfxQueue();

	Result = InDevice->CreateCommandQueue(&InDescription, __uuidof(ID3D12CommandQueue), (void**)&(*OutQueue)->Queue);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	return EResult_OK;
}


CGfxQueue::CGfxQueue()
{
	this->Queue = nullptr;
}

CGfxQueue::~CGfxQueue()
{

}

void CGfxQueue::Shutdown()
{
	if (this->Queue)
	{
		this->Queue->Release();
		this->Queue = nullptr;
	}
}
