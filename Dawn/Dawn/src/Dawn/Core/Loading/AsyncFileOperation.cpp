//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "AsyncFileOperation.h"
#include "File.h"
#include "inc_common.h"

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Creates a new asynchronous file operation with a given thread you need 
	// to supply a pointer to a skyTransferredBytesData struct in order to 
	// retrieve the read or write bytes at the end of the operation.
	//-----------------------------------------------------------------------------
	CAsyncFileOperation::CAsyncFileOperation(TThread<CFile, SFileThreadData>* InThread, STransferredBytesData* InData)
	{
		this->Thread = nullptr;

		if (this->Thread)
			this->Thread = InThread;

		if (InData)
			this->Data = InData;
	}

	//-----------------------------------------------------------------------------
	// Destructs the object. If the thread isn't finished yet it will wait 
	// until its finished and after that deletes the thread.
	//-----------------------------------------------------------------------------
	CAsyncFileOperation::~CAsyncFileOperation(void)
	{
		if (this->Thread)
		{
			if (!this->IsFinished())
				this->WaitUntilFinished();

			SafeDelete(Data);
			SafeDelete(Thread);
		}
	}

	//-----------------------------------------------------------------------------
	// Destructs the object. Call this only if youre sure the thread has finished
	// otherwise call cancel!
	//-----------------------------------------------------------------------------
	void CAsyncFileOperation::Close(void)
	{
		SafeDelete(Data);
		SafeDelete(Thread);
	}

	//-----------------------------------------------------------------------------
	// Checks whether or not the thread is still active or not.
	//-----------------------------------------------------------------------------
	bool CAsyncFileOperation::IsFinished(void) const
	{
		if (!Thread)
			return true;

		return Thread->IsActive();
	}

	//-----------------------------------------------------------------------------
	// Waits till the thread is finished. Returns the processed bytes or -1 if 
	// anything was wrong. 
	//-----------------------------------------------------------------------------
	size_t CAsyncFileOperation::WaitUntilFinished(void) const
	{
		size_t Size = -1;

		if (!Thread)
			return Size;

		// wait as long as the thread is still active!
		while (Thread->IsActive());


		if (this->Data)
			Size = Data->Size;


		return Size;
	}

	//-----------------------------------------------------------------------------
	// Cancels the thread.
	//-----------------------------------------------------------------------------
	void CAsyncFileOperation::Cancel(void)
	{
		if (Thread)
			Thread->Terminate();
	}
}