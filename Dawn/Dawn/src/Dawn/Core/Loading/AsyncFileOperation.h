#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

#include "inc_common.h"
#include "Core/Threading/Thread.h"
#include "File.h"

namespace Dawn
{
	class CFileThread;

	//-----------------------------------------------------------------------------
	// Main Interface Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// An asynchron file operation will be returned from the File 
	// whenever one calls AsyncRead or AsyncWrite. This class takes care of 
	// cleaning up the thread class and so on.
	//-----------------------------------------------------------------------------
	class AsyncFileOperation
	{
	public:
		//-----------------------------------------------------------------------------
		// Constructor & Destructor
		//-----------------------------------------------------------------------------
		AsyncFileOperation(TThread<File, SFileThreadData>* InThread, STransferredBytesData* InData);
		~AsyncFileOperation(void);

		//-----------------------------------------------------------------------------
		// Public Methods
		//-----------------------------------------------------------------------------
		bool AsyncFileOperation::IsFinished(void) const;
		size_t AsyncFileOperation::WaitUntilFinished(void) const;
		void AsyncFileOperation::Cancel(void);
		void AsyncFileOperation::Close(void);

	private:
		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
		TThread<File, SFileThreadData>* Thread;
		STransferredBytesData* Data;
	};
}