#pragma once
#include "Core/Timer.h"

namespace Dawn
{
	class GfxGDI;
	class Event;
	class ResourceSystem;
	class PhysicsWorld;
	class DeferredRenderer;
	class Window;
	class Layer;
	class World;


	class DAWN_API Application : public std::enable_shared_from_this<Application>
	{
	public:
		Application(AppSettings& InSettings);
		virtual ~Application();
		void Run();

		static AppSettings* GetSettings();

	public:
		Shared<GfxGDI> GetGDI();
		Shared<DeferredRenderer> GetRenderer();
		Shared<World> GetWorld();
		Shared<World> GetEditorWorld();
		Shared<PhysicsWorld> GetPhysics();
		Shared<ResourceSystem> GetResourceSystem();
		Window* GetWindow() const;
		bool GetIsInEditMode() const { return bIsInEditMode;}

	protected:
		std::vector<Layer*> Layers;
		std::vector<Layer*>::iterator LayerInsertCount;
		bool IsInitialized = false;
		bool bIsInEditMode = false;
		Time Time;
		Shared<DeferredRenderer> Renderer;
		Shared<GfxGDI> GDI;
		Shared<World> World;
		Shared<Dawn::World> EditorWorld;
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

		void CalculateSceneGraph(Dawn::World* InWorld);

		void PushLayer(Layer* InLayer);
		void PopLayer(Layer* InLayer);
		void ClearLayers();

		std::vector<Layer*>::iterator begin();
		std::vector<Layer*>::iterator end();
		void Tick();
	};

	Application* CreateApplication();

	extern DAWN_API Shared<Application> g_Application;
}


