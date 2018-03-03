#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

#include "../../../inc_common.h"
#include "../../Threading/Thread.h"
#include "File.h"

class CFileThread;

//-----------------------------------------------------------------------------
// Main Interface Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// An asynchron file operation will be returned from the CFile 
// whenever one calls AsyncRead or AsyncWrite. This class takes care of 
// cleaning up the thread class and so on.
//-----------------------------------------------------------------------------
class CAsyncFileOperation 
{
public:
	//-----------------------------------------------------------------------------
	// Constructor & Destructor
	//-----------------------------------------------------------------------------
	CAsyncFileOperation(TThread<CFile, SFileThreadData>* InThread, STransferredBytesData* InData);
	~CAsyncFileOperation(void);

	//-----------------------------------------------------------------------------
	// Public Methods
	//-----------------------------------------------------------------------------
	bool CAsyncFileOperation::IsFinished ( void ) const;
	size_t CAsyncFileOperation::WaitUntilFinished ( void ) const;
	void CAsyncFileOperation::Cancel ( void );
	void CAsyncFileOperation::Close(void);

private:
	//-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
	TThread<CFile, SFileThreadData>* Thread;
	STransferredBytesData* Data;
};