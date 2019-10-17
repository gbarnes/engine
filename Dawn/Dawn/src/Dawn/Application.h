#pragma once

#include "inc_core.h"
#include "Layer.h"

#include "Core/Window.h"
#include "ResourceSystem/ResourceSystem.h"
#include "EntitySystem/World.h"

namespace Dawn
{
	class GfxGDI;
	class Event;
	class ResourceSystem;
	class PhysicsWorld;
	class DeferredRenderer;


	class DAWN_API Application : public std::enable_shared_from_this<Application>
	{
	public:
		Application(AppSettings& InSettings);
		virtual ~Application();
		void Run();

		static AppSettings* GetSettings();

		Shared<GfxGDI> GetGDI()
		{
			return GDI;
		}

		Shared<DeferredRenderer> GetRenderer()
		{
			return Renderer;
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
		std::vector<Layer*> Layers;
		std::vector<Layer*>::iterator LayerInsertCount;
		bool IsInitialized = false;
		Time Time;
		Shared<DeferredRenderer> Renderer;
		Shared<GfxGDI> GDI;
		Shared<World> World;
		Shared<ResourceSystem> ResourceSystem;
		Shared<PhysicsWorld> Physics;
		Unique<Window> Window;
		AppSettings Settings;

		
		virtual void SetupLayers();
		virtual void Load() = 0;

		virtual void Update(float InDeltaTime) = 0;
		virtual void FixedUpdate(float InFixedTime) = 0;
		virtual void Render();
		virtual void Cleanup() = 0;
		virtual void Resize(int width, int height);

		void PushLayer(Layer* InLayer);
		void PopLayer(Layer* InLayer);
		void ClearLayers();

		std::vector<Layer*>::iterator begin() { return Layers.begin(); }
		std::vector<Layer*>::iterator end() { return Layers.end(); }
		void Tick();
	};

	Application* CreateApplication();

	extern DAWN_API Shared<Application> g_Application;
}


