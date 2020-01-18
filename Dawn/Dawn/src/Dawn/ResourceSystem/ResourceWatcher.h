#pragma once

namespace Dawn
{
	struct DAWN_API ResourceWatchData
	{
		Path DirectoryPath;
		std::thread ObserverThread;
		HANDLE TermEvent;
		HANDLE DirHandle;
		bool bIsRunning = false;
	};

	class ResourceSystem;

	namespace ResourceWatcher
	{
		ResourceWatchData* StartMonitoring(Shared<ResourceSystem>& InResourceSystem, Path& InPath);
		std::set<std::string> GrabLatestFileChanges(ResourceWatchData* InData);
		void StopMonitoring(ResourceWatchData* InData);
	}
}