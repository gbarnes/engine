#pragma once

/*#include "Core/Logging/Log.h"
#include "Core/System/Window.h"
#include "Core/System/EObject.h"
#include "Core/System/Locator.h"*/

#include "Core/Window.h"
#include "inc_core.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Layer.h"
#include <vector>


#define AppLocatorId std::string("App")

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

	class Event;
	class CGfxRootSignature;
	

	class DAWN_API Application : public EObject
	{
	public:
		Application(SAppSettings& InSettings);
		virtual ~Application();
		void Run();

		inline static RefPtr<Application> Get()
		{
			return RefPtr<Application>(Locator::Get<Application>(AppLocatorId));
		}

		inline static SAppSettings* GetSettings()
		{
			return &Get()->Settings;
		}

		void OnMouseMovedEvent(Event& InEvent);
		void OnMousePressedEvent(Event& InEvent);
		void OnMouseReleasedEvent(Event& InEvent);

		static uint64_t GetFrameCount()
		{
			return FrameCount;
		}

	protected:
		Window Window;
		SAppSettings Settings;
		Timer Clock;

		virtual void SetupLayers();
		virtual void Tick();
		virtual void Load();

		void PushLayer(Layer* InLayer);
		void PopLayer(Layer* InLayer);
		void ClearLayers();

		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }

	private:
		std::vector<Layer*> Layers;
		std::vector<Layer*>::iterator LayerInsertCount;

		static uint64_t FrameCount;
	};

	typedef RefPtr<Application> ReferenceAppPtr;

	Application* CreateApplication();
}


