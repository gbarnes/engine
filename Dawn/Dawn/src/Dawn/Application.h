#pragma once

/*#include "Core/Logging/Log.h"
#include "Core/System/Window.h"
#include "Core/System/EObject.h"
#include "Core/System/Locator.h"*/

#include "Core/Window.h"
#include "inc_core.h"

namespace Dawn
{
	struct SAppSettings
	{
		std::wstring Title;
		u32 WindowWidth;
		u32 WindowHeight;
		bool IsFullscreen;
		bool UseVsync;
	};

	class DAWN_API Application : public CEObject
	{
	public:
		Application(SAppSettings& InSettings);
		virtual ~Application();
		void Run();

		inline static CRefPtr<Application> Get()
		{
			Application* App = dynamic_cast<Application*>(CLocator::Get(std::string("App")));
			return CRefPtr<Application>(App);
		}

	protected:
		CWindow Window;
		SAppSettings Settings;

		virtual void Tick();
	};

	typedef CRefPtr<Application> CReferenceAppPtr;

	Application* CreateApplication();
}


