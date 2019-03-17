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

	class CEvent;
	class CGfxRootSignature;
	

	class DAWN_API Application : public CEObject
	{
	public:
		Application(SAppSettings& InSettings);
		virtual ~Application();
		void Run();

		inline static CRefPtr<Application> Get()
		{
			return CRefPtr<Application>(CLocator::Get<Application>(AppLocatorId));
		}

		inline static SAppSettings* GetSettings()
		{
			return &Get()->Settings;
		}

		void OnMouseMovedEvent(CEvent& InEvent);
		void OnMousePressedEvent(CEvent& InEvent);
		void OnMouseReleasedEvent(CEvent& InEvent);

	protected:
		CWindow Window;
		SAppSettings Settings;

		virtual void SetupLayers();
		virtual void Tick();
		virtual void Load();

		void PushLayer(CLayer* InLayer);
		void PopLayer(CLayer* InLayer);
		void ClearLayers();

		std::vector<CLayer*>::iterator begin() { return Layers.begin(); }
		std::vector<CLayer*>::iterator end() { return Layers.end(); }

	private:
		std::vector<CLayer*> Layers;
		std::vector<CLayer*>::iterator LayerInsertCount;
	};

	typedef CRefPtr<Application> CReferenceAppPtr;

	Application* CreateApplication();
}


