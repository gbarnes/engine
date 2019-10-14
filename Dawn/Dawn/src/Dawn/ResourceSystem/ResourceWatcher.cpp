#pragma once
#include "ResourceWatcher.h"
#include "inc_common.h"
#include "inc_core.h"
#include "ResourceSystem.h"

namespace Dawn
{

	std::mutex FileSynchroMutex;
	std::set<std::string> FilesToProcess;

	void WatchPathForFileChanges(const Shared<ResourceSystem>& InResourceSystem, ResourceWatchData* InData, Path& InPath)
	{
		DWORD dwBytes = 0;
		std::vector<BYTE> buffer(1024 * 64);
		OVERLAPPED o{ 0 };
		bool bPending = false, bKeepRunning = true;

		if (InData->DirHandle == INVALID_HANDLE_VALUE)
		{
			DWN_CORE_ERROR("Invalid Directory Handle!");
			return;
		}

		if (InData->TermEvent == INVALID_HANDLE_VALUE)
		{
			DWN_CORE_ERROR("Invalid Event Handle!");
			return;
		}

		//Be sure to set the hEvent member of the OVERLAPPED structure to a unique event.
		o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!o.hEvent) {
			
		}
		HANDLE h[2] = { o.hEvent, InData->TermEvent };

		do
		{
			bPending = ReadDirectoryChangesW(InData->DirHandle,
				&buffer[0], buffer.size(),
				TRUE, FILE_NOTIFY_CHANGE_FILE_NAME |
				 FILE_NOTIFY_CHANGE_SIZE,
				&dwBytes, &o, NULL);
			if (!bPending)
			{
				
			}

			switch (WaitForMultipleObjects(2, h, FALSE, INFINITE))
			{
			case WAIT_OBJECT_0:
			{
				if (!GetOverlappedResult(InData->DirHandle, &o, &dwBytes, TRUE)) {
					
				}

				bPending = false;

				if (dwBytes == 0)
					break;

				FILE_NOTIFY_INFORMATION *fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[0]);
				do
				{
					if (fni->Action != 0)
					{
						std::wstring wsFileName(fni->FileName);
						std::string FileName = std::string(wsFileName.begin(), wsFileName.end());
						
						// Todo: add support to load new created files or remove deleted
						//		 files later on. (gb, 10/12/19)
						Path NewPath(InPath);
						NewPath.append(FileName);
						if (std::filesystem::is_regular_file(NewPath))
						{
							std::lock_guard lock(FileSynchroMutex);
							FilesToProcess.insert(FileName);
						}
					}

					if (fni->NextEntryOffset == 0) {
						buffer = std::vector<BYTE>(1024 * 64);
						break;
					}

					fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
				} while (true);

				break;
			}

			case WAIT_OBJECT_0 + 1:
				bKeepRunning = false;
				break;

			case WAIT_FAILED:
				break;
			}
		} while (bKeepRunning);

		if (bPending)
		{
			CancelIo(InData->DirHandle);
			GetOverlappedResult(InData->DirHandle, &o, &dwBytes, TRUE);
		}

		CloseHandle(o.hEvent);
	}

	ResourceWatchData* ResourceWatcher::StartMonitoring(Shared<ResourceSystem>& InResourceSystem, Path& InPath)
	{
		ResourceWatchData* WatchData = new ResourceWatchData();
		WatchData->DirectoryPath = InPath;
		WatchData->bIsRunning = true;
		WatchData->DirHandle = CreateFileW(
			InPath.c_str(),
			FILE_LIST_DIRECTORY | GENERIC_READ,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);
		WatchData->TermEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		WatchData->ObserverThread = std::thread(WatchPathForFileChanges, InResourceSystem, WatchData, InPath);
		return WatchData;
	}

	std::set<std::string> ResourceWatcher::GrabLatestFileChanges(ResourceWatchData* InData)
	{

		std::set<std::string> TempSet;

		if(InData)
		{
			if (InData->bIsRunning)
			{
				TempSet = std::set<std::string>(FilesToProcess);

				std::lock_guard lock(FileSynchroMutex);
				FilesToProcess.clear();
			}
		}

		return TempSet;
	}

	void ResourceWatcher::StopMonitoring(ResourceWatchData* InData)
	{
		if (InData)
		{
			SetEvent(InData->TermEvent);
			InData->ObserverThread.join();
			InData->bIsRunning = false;
		}
	}
}