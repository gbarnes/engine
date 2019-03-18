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
	class AsyncFileOperation;
	class CriticalSection;

	typedef std::shared_ptr<AsyncFileOperation> CSharedAsyncFileOperation;

	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// This class provides both asynchronous
	// and synchronous read and write operations for files. Furthermore files can
	// be used in binary or ascii mode.
	//-----------------------------------------------------------------------------
	class File
	{
	public:
		//-------------------------------------------------------------------------
		// Constructor & Destructor
		//-------------------------------------------------------------------------
		File::File();
		File::~File(void);


		//-------------------------------------------------------------------------
		// Synchronous Methods
		//-------------------------------------------------------------------------
		EResult File::Open(std::string InPath, int InMode, bool InIsAsync);
		void File::Close();

		size_t File::Read(void* InBuffer, size_t InLength);
		size_t File::Write(const void* InBuffer, size_t InLength);

		//-------------------------------------------------------------------------
		// Asynchronous Methods
		//-------------------------------------------------------------------------
		CSharedAsyncFileOperation File::ReadAsync(void* InBuffer, size_t InLength);
		CSharedAsyncFileOperation File::WriteAsync(void* InBuffer, size_t InLength);

		//-------------------------------------------------------------------------
		// Public Mehtods
		//-------------------------------------------------------------------------
		void File::Seek(size_t InPosition);
		void File::SeekToEnd(void);
		void File::Skip(size_t InBytes);
		size_t File::Tell(void) const;

		//-------------------------------------------------------------------------
		// Public Static Mehtods
		//-------------------------------------------------------------------------
		static std::string File::GetStringFromMode(int InMode);

	private:

		//-------------------------------------------------------------------------
		// Private Members
		//-------------------------------------------------------------------------
		std::string Path;
		int Mode;
		bool IsAsync;
		EFileMode FileMode;
		FILE* WinFile;
		CriticalSection* CriticalSection;

		//-------------------------------------------------------------------------
		// Private Methods
		//-------------------------------------------------------------------------
		EFileMode File::GetFileModeFromInt(int InMode);
		void File::ReadBufferAsync(SFileThreadData InData);
		void File::WriteBufferAsync(SFileThreadData InData);
	};
}

