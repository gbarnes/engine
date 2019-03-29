#include "Application.h"
#include "imgui.h"

#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "inc_common.h"
#include "Layers/ImGuiLayer.h"
#include "Layers/TestRenderLayer.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceLoaders.h"
#include "debug_draw.h"
#include "brofiler.h"
#include "JobSystem/JobSystem.h"
#include "Rendering/RenderResourceHelper.h"

#define USE_OPENGL_GFX
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	SEventHandle g_MouseMovedEvtHandle, g_MousePressedHandle, g_MouseReleasedHandle;
	uint64_t Application::FrameCount = 0;

	Application::Application(AppSettings& InSettings)
	{
		this->Settings = InSettings;
		Locator::Add(AppLocatorId, this);
	}

	Application::~Application()
	{
		Locator::Remove(AppLocatorId);
	}


	void Application::Run()
	{
		// Resource System initialization
		if (!ResourceSystem.Initialize("E:/Git/engine/Dawn/Assets/", { ".obj", ".jpg", ".png", ".shader" }))
		{
			DWN_CORE_ERROR("Couldn't initialize resource system");
			system("pause");
			return;
		}

		ResourceSystem.AddRef();
		ResourceSystem.RegisterLoader(ResourceType_StaticMesh, BIND_FS_LOADER(Dawn::RS_LoadStaticMeshWithAssimp));
		ResourceSystem.RegisterLoader(ResourceType_Shader, BIND_FS_LOADER(Dawn::RS_LoadShader));
		ResourceSystem.RegisterLoader(ResourceType_Image, BIND_FS_LOADER(Dawn::RS_LoadImage));

		if (!ResourceSystem.BuildDatabase())
		{
			DWN_CORE_ERROR("Couldn't build database for resource system");
			system("pause");
			return;
		}

		// Window Creation Setup
		EResult Result = this->Window.Initialize(Settings.Title, Settings.Width,
								Settings.Height, Settings.IsFullscreen, 
								Settings.ColorBits, Settings.DepthBits, 
								Settings.AlphaBits);

		this->Window.OnWindowPaint = std::bind(&Application::Tick, this);

		if (Result != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize window");
			system("pause");
			return;
		}
		
		if (Window.Create() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't create window system");
			system("pause");
			return;
		}
		Settings.Hwnd = Window.GetHwnd();

		GDI = CreateGDI();
		if (!GDI->Init(Settings))
		{
			DWN_CORE_ERROR("Couldn't initialize GDI!\n");
			system("pause");
			return;
		}

		if (JobSystem::Initialize() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize job system!\n");
			system("pause");
			return;
		}

		{
			Load();
			SetupLayers();
		}

		DWN_CORE_INFO("Core Context initialized.");
		
		IsInitialized = true;
		Clock.Reset();

		while (true)
		{
			if (Window.PeekMessages())
				break;
		}

		ClearLayers();
		GDI->Shutdown();
		JobSystem::Shutdown();
		ResourceSystem.Shutdown();

		DWN_CORE_INFO("Core Context shutdown.");
	}

	void Application::Load()
	{
		RenderResourceHelper::LoadCommonShaders();
	}

	void Application::Tick()
	{BROFILER_FRAME("MainThread")
		
		if (!IsInitialized)
			return;

		++Application::FrameCount;

		Clock.Tick();

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, Settings.Width, Settings.Height);
		glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (Layer* layer : Layers)
		{
			layer->Update();
			layer->Render();
		}

		GDI->Present();
	}

	void Application::SetupLayers()
	{
		LayerInsertCount = Layers.begin();

		this->PushLayer(new ImGuiLayer(Window.GetHwnd()));
		this->PushLayer(new TestRenderLayer());

		for (Layer* layer : Layers)
			layer->Setup();
	}

	void Application::PushLayer(Layer* InLayer)
	{
		LayerInsertCount = Layers.emplace(LayerInsertCount, InLayer);
	}

	void Application::PopLayer(Layer* InLayer)
	{
		auto it = std::find(this->Layers.begin(), this->Layers.end(), InLayer);
		if (it != Layers.end())
		{
			Layers.erase(it);
			LayerInsertCount--;
		}
	}

	void Application::ClearLayers()
	{
		for (Layer* layer : Layers) 
		{
			layer->Free();
			delete layer;
		}

		Layers.clear();
	}
}
