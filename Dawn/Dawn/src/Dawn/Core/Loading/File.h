#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

#include "inc_common.h"

namespace Dawn
{
	//-----------------------------------------------------------------------------
	// Structs 
	//-----------------------------------------------------------------------------
	struct STransferredBytesData
	{
		size_t Size;
	};

	struct SFileThreadData
	{
		void* Buffer;
		size_t Length;
		STransferredBytesData* Data;
	};


	typedef enum EFileMode
	{
		EFileMode_Read = 0x01,
		EFileMode_Write = 0x02,
		EFileMode_Recreate = 0x05,
		EFileMode_Append = 0x08,
		EFileMode_Binary = 0x10,
		EFileMode_Ascii = 0x20
	} EFileMode;



	//-----------------------------------------------------------------------------
	// Type definitions and forward declarations. 
	//-----------------------------------------------------------------------------
	class CAsyncFileOperation;
	class CCriticalSection;

	typedef std::shared_ptr<CAsyncFileOperation> CSharedAsyncFileOperation;

	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// This class provides both asynchronous
	// and synchronous read and write operations for files. Furthermore files can
	// be used in binary or ascii mode.
	//-----------------------------------------------------------------------------
	class CFile
	{
	public:
		//-------------------------------------------------------------------------
		// Constructor & Destructor
		//-------------------------------------------------------------------------
		CFile::CFile();
		CFile::~CFile(void);


		//-------------------------------------------------------------------------
		// Synchronous Methods
		//-------------------------------------------------------------------------
		EResult CFile::Open(std::string InPath, int InMode, bool InIsAsync);
		void CFile::Close();

		size_t CFile::Read(void* InBuffer, size_t InLength);
		size_t CFile::Write(const void* InBuffer, size_t InLength);

		//-------------------------------------------------------------------------
		// Asynchronous Methods
		//-------------------------------------------------------------------------
		CSharedAsyncFileOperation CFile::ReadAsync(void* InBuffer, size_t InLength);
		CSharedAsyncFileOperation CFile::WriteAsync(void* InBuffer, size_t InLength);

		//-------------------------------------------------------------------------
		// Public Mehtods
		//-------------------------------------------------------------------------
		void CFile::Seek(size_t InPosition);
		void CFile::SeekToEnd(void);
		void CFile::Skip(size_t InBytes);
		size_t CFile::Tell(void) const;

		//-------------------------------------------------------------------------
		// Public Static Mehtods
		//-------------------------------------------------------------------------
		static std::string CFile::GetStringFromMode(int InMode);

	private:

		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
		std::string Path;
		int Mode;
		bool IsAsync;
		EFileMode FileMode;
		FILE* File;
		CCriticalSection* CriticalSection;

		//-------------------------------------------------------------------------
		// Private Methods
		//-------------------------------------------------------------------------
		EFileMode CFile::GetFileModeFromInt(int InMode);
		void CFile::ReadBufferAsync(SFileThreadData InData);
		void CFile::WriteBufferAsync(SFileThreadData InData);
	};
}

