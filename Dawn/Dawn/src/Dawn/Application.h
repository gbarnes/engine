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


	class DAWN_API Application : public std::enable_shared_from_this<Application>
	{
	public:
		Application(AppSettings& InSettings);
		virtual ~Application();
		void Run();

		static AppSettings* GetSettings();

		static uint64_t GetFrameCount()
		{
			return FrameCount;
		}

		Shared<GfxGDI> GetGDI()
		{
			return GDI;
		}
		
		Shared<World> GetWorld()
		{
			return World;
		}

		Shared<PhysicsWorld> GetPhysics()
		{
			return Physics;
		}

		Shared<ResourceSystem> GetResourceSystem()
		{
			return ResourceSystem;
		}

		Window* GetWindow() const
		{
			return Window.get();
		}

	protected:
		Shared<GfxGDI> GDI;
		Shared<World> World;
		Shared<ResourceSystem> ResourceSystem;
		Shared<PhysicsWorld> Physics;
		Unique<Window> Window;

		physx::PxFoundation* Physx;
		AppSettings Settings;
		Timer Clock;

		virtual void SetupLayers();
		virtual void Tick();
		virtual void Load();

		virtual void Resize(int width, int height);

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

	Application* CreateApplication();

	extern DAWN_API Shared<Application> g_Application;
}


