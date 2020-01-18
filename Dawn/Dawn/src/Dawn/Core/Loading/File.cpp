//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
//#include "..\FileSystem\skyAsyncFileOperation.h"
#include "stdafx.h"
#include "File.h"
#include "../Threading/CriticalSection.h"
#include "AsyncFileOperation.h"

namespace Dawn
{

	//-----------------------------------------------------------------------------
	// Creates a file container which can be used to load content from a file
	// Call open before reading from a file and close afterwards!
	//-----------------------------------------------------------------------------
	File::File()
	{
		this->Path = "";
		this->Mode = 0;
		this->FileMode = GetFileModeFromInt(Mode);
		this->IsAsync = false;
		this->CriticalSection = nullptr;
		this->WinFile = nullptr;

	}

	//-----------------------------------------------------------------------------
	// Destructs the object and will close the stream.
	//-----------------------------------------------------------------------------
	File::~File(void)
	{
		Close();
		SafeDelete(this->CriticalSection);
	}


	///////////////////////////////////////////////////////////////////////////////
	// File Member Definitions
	///////////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------------
	// Opens a given file for reading. Async mode is still in progress!
	//-----------------------------------------------------------------------------
	EResult File::Open(std::string InPath, int InMode, bool InIsAsync)
	{
		this->IsAsync = InIsAsync;

		// once we declared this file as asynchron we need to create a critical section
		if (InIsAsync)
		{
			this->CriticalSection = new Dawn::CriticalSection();
		}

		std::string stringMode = File::GetStringFromMode(InMode);

		errno_t error = fopen_s(&WinFile, InPath.c_str(), stringMode.c_str());
		if (error)
		{
			char errorMessage[256];
			int i = 0;
			strerror_s(errorMessage, 100, i);
			std::cout << errorMessage << "\n";
			//SKY_PRINT_CONSOLE("skyPlatformFile:", "couldn't load file at path " + path + " with error message -" + skyToString(errorMessage), 0x0A);
			return EResult_ERROR;
		}



		return EResult_OK;
	}

	void File::Close()
	{
		if (this->WinFile != nullptr)
			fclose(this->WinFile);
	}

	//-----------------------------------------------------------------------------
	// Reads a specified length of bytes into a given void* buffer.
	//-----------------------------------------------------------------------------
	size_t File::Read(void* InBuffer, size_t InLength)
	{
		if (!WinFile)
			return -1;

		rewind(WinFile);
		size_t Value = fread(InBuffer, sizeof(byte), InLength, WinFile);

		return Value;
	}

	//-----------------------------------------------------------------------------
	// Writes a given length of bytes from a void* buffer into a file.
	//-----------------------------------------------------------------------------
	size_t File::Write(const void* InBuffer, size_t InLength)
	{
		if (!WinFile)
			return -1;

		size_t value = fwrite(InBuffer, sizeof(byte), InLength, WinFile);
		return value;
	}

	//-----------------------------------------------------------------------------
	// Sets the position indicator associated with the stream to a new position.
	//-----------------------------------------------------------------------------
	void File::Seek(size_t InPosition)
	{
		if (WinFile)
		{
			fseek(WinFile, InPosition, SEEK_SET);
		}
	}

	//-----------------------------------------------------------------------------
	// Sets the position indicator associated with the stream to the end of the file.
	//-----------------------------------------------------------------------------
	void File::SeekToEnd(void)
	{
		if (WinFile)
		{
			fseek(WinFile, SEEK_CUR, SEEK_END);
		}
	}

	//-----------------------------------------------------------------------------
	// Not implemented yet!
	//-----------------------------------------------------------------------------
	TODO("Still needs to be implemented.")
		void File::Skip(size_t bytes)
	{
		//SKY_PRINT_CONSOLE("skyPlatformFile", "Skip-Function isn't implemented yet.", 0x0A);
	}

	//-----------------------------------------------------------------------------
	// Returns the current value of the position indicator of the stream.
	//-----------------------------------------------------------------------------
	size_t File::Tell(void) const
	{
		if (!WinFile)
			return -1;

		return ftell(this->WinFile);
	}

	//-----------------------------------------------------------------------------
	// Reads a specific length into a given void buffer in an asynchronous fashion.
	// This will return an AsynchronousFileOperation instance which takes care 
	// of handling all the thread stuff.
	//-----------------------------------------------------------------------------
	CSharedAsyncFileOperation File::ReadAsync(void* InBuffer, size_t InLength)
	{
		if (this->IsAsync)
		{
			TThread<File, SFileThreadData>* Thread = new TThread<File, SFileThreadData>();

			STransferredBytesData* TransferData = new STransferredBytesData;
			TransferData->Size = -1;

			SFileThreadData Data;
			Data.Buffer = InBuffer;
			Data.Length = InLength;
			Data.Data = TransferData;

			Thread->Run(this, &File::ReadBufferAsync, Data);

			CSharedAsyncFileOperation Operation = CSharedAsyncFileOperation(new AsyncFileOperation(Thread, TransferData));
			return Operation;
		}

		return CSharedAsyncFileOperation();
	}

	//-----------------------------------------------------------------------------
	// Writes a specific length from a given void buffer to a file in an asynchronous fashion.
	// This will return an AsynchronousFileOperation instance which takes care 
	// of handling all the thread stuff.
	//-----------------------------------------------------------------------------
	CSharedAsyncFileOperation File::WriteAsync(void* InBuffer, size_t InLength)
	{
		if (this->IsAsync)
		{
			TODO("Put this into the Async  File Operation since we have it there anyway and allocate it on the stack!")
				TThread<File, SFileThreadData>* Thread = new TThread<File, SFileThreadData>();

			STransferredBytesData* TransferData = new STransferredBytesData;
			TransferData->Size = -1;

			SFileThreadData Data;
			Data.Buffer = InBuffer;
			Data.Length = InLength;
			Data.Data = TransferData;

			Thread->Run(this, &File::WriteBufferAsync, Data);

			CSharedAsyncFileOperation Operation = CSharedAsyncFileOperation(new AsyncFileOperation(Thread, TransferData));
			return Operation;
		}

		return CSharedAsyncFileOperation();
	}

	//-----------------------------------------------------------------------------
	// Helper function which takes care of calling the Read-Method.
	//-----------------------------------------------------------------------------
	void File::ReadBufferAsync(SFileThreadData InData)
	{
		if (WinFile == nullptr)
			return;

		{
			Dawn::CriticalSection criticalSection;
		
			rewind(WinFile);
			size_t TransferredSize = fread(InData.Buffer, sizeof(byte), InData.Length, WinFile);
			if (InData.Data != nullptr)
				InData.Data->Size = TransferredSize;
		}
	}

	//-----------------------------------------------------------------------------
	//  Name : WriteBufferAsync (Function)
	/// <summary>
	/// Helper function which takes care of calling the Write-Method.
	/// </summary>
	//-----------------------------------------------------------------------------
	void File::WriteBufferAsync(SFileThreadData InData)
	{
		if (!WinFile)
			return;

		{
			Dawn::CriticalSection criticalSection;
		
			size_t TransferredSize = fwrite(InData.Buffer, sizeof(byte), InData.Length, WinFile);
			if (InData.Data != nullptr)
				InData.Data->Size = TransferredSize;
		}
	}

	//-----------------------------------------------------------------------------
	/// Returns the correct file mode from the enum EFileMode. 
	//-----------------------------------------------------------------------------
	std::string File::GetStringFromMode(int InMode)
	{
		std::string Mode = "r";

		if (InMode)
		{
			Mode = "";

			if (HAS_FLAG(InMode, EFileMode_Read))
			{
				Mode = "r";
				//mFileMode = skyFileMode::eFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Read) && HAS_FLAG(InMode, EFileMode_Ascii))
			{
				Mode = "r";
				//mFileMode = skyFileMode::eFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Read) && HAS_FLAG(InMode, EFileMode_Binary))
			{
				Mode = "rb";
				//mFileMode = skyFileMode::eFileMode_Binary;
			}

			if (HAS_FLAG(InMode, EFileMode_Write))
			{
				Mode += "w";
				//mFileMode = skyFileMode::eFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Write) && HAS_FLAG(InMode, EFileMode_Binary))
			{
				Mode = "wb";
				//mFileMode = skyFileMode::eFileMode_Binary;
			}

			if (HAS_FLAG(InMode, EFileMode_Write) && HAS_FLAG(InMode, EFileMode_Recreate))
			{
				Mode = "w+";
				//mFileMode = skyFileMode::eFileMode_Ascii;
			}

			TODO("Add the rest of the modes");

			/*
			if(mode == (skyFileMode::eFileMode_Write | skyFileMode::eFileMode_Recreate | skyFileMode::eFileMode_Binary))
			{
				mMode = "w+b";
				mFileMode = skyFileMode::eFileMode_Binary;
			}

			if(mode == (skyFileMode::eFileMode_Write  | skyFileMode::eFileMode_Append) || mode == (skyFileMode::eFileMode_Append))
			{
				mMode = "a";
				mFileMode = skyFileMode::eFileMode_Ascii;
			}

			if(mode == (skyFileMode::eFileMode_Write | skyFileMode::eFileMode_Append | skyFileMode::eFileMode_Binary))
			{
				mMode = "ab";
				mFileMode = skyFileMode::eFileMode_Binary;
			}*/
		}

		return Mode;
	}

	EFileMode File::GetFileModeFromInt(int InMode)
	{
		EFileMode FileMode = EFileMode_Ascii;

		if (InMode)
		{
			if (HAS_FLAG(InMode, EFileMode_Read))
			{
				FileMode = EFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Read) && HAS_FLAG(InMode, EFileMode_Ascii))
			{
				FileMode = EFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Read) && HAS_FLAG(InMode, EFileMode_Binary))
			{
				FileMode = EFileMode_Binary;
			}

			if (HAS_FLAG(InMode, EFileMode_Write))
			{
				FileMode = EFileMode_Ascii;
			}

			if (HAS_FLAG(InMode, EFileMode_Write) && HAS_FLAG(InMode, EFileMode_Binary))
			{
				FileMode = EFileMode_Binary;
			}

			if (HAS_FLAG(InMode, EFileMode_Write) && HAS_FLAG(InMode, EFileMode_Recreate))
			{
				FileMode = EFileMode_Ascii;
			}

			TODO("Add the rest of the modes");

			/*
			if(mode == (skyFileMode::eFileMode_Write | skyFileMode::eFileMode_Recreate | skyFileMode::eFileMode_Binary))
			{
				mMode = "w+b";
				mFileMode = skyFileMode::eFileMode_Binary;
			}

			if(mode == (skyFileMode::eFileMode_Write  | skyFileMode::eFileMode_Append) || mode == (skyFileMode::eFileMode_Append))
			{
				mMode = "a";
				mFileMode = skyFileMode::eFileMode_Ascii;
			}

			if(mode == (skyFileMode::eFileMode_Write | skyFileMode::eFileMode_Append | skyFileMode::eFileMode_Binary))
			{
				mMode = "ab";
				mFileMode = skyFileMode::eFileMode_Binary;
			}*/
		}

		return FileMode;
	}
}
