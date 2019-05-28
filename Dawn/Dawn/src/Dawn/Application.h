#pragma once

#include "inc_core.h"
#include "Layer.h"

#include "Core/Window.h"
#include "ResourceSystem/ResourceSystem.h"
#include "EntitySystem/World.h"

#define AppLocatorId std::string("App")

namespace Dawn
{

	class GfxGDI;
	class Event;
	class ResourceSystem;
	class PhysicsWorld;

	class DAWN_API Application : public EObject
	{
	public:
		Application(AppSettings& InSettings);
		virtual ~Application();
		void Run();

		inline static RefPtr<Application> Get()
		{
			return RefPtr<Application>(Locator::Get<Application>(AppLocatorId));
		}

		inline static AppSettings* GetSettings()
		{
			return &Get()->Settings;
		}

		static uint64_t GetFrameCount()
		{
			return FrameCount;
		}

	protected:
		std::unique_ptr<GfxGDI> GDI;
		std::unique_ptr<World> World;
		std::unique_ptr<Window> Window;
		std::unique_ptr<PhysicsWorld> Physics;

		physx::PxFoundation* Physx;
		ResourceSystem ResourceSystem;
		AppSettings Settings;
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
		bool IsInitialized = false;
		static uint64_t FrameCount;
	};

	typedef RefPtr<Application> ReferenceAppPtr;

	Application* CreateApplication();
}


