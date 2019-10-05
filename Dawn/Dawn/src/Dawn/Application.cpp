#include "Application.h"
#include "imgui.h"

#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "inc_common.h"
#include "Layers/ImGuiLayer.h"
#include "Layers/TestRenderLayer.h"
#include "Layers/WorldSimulateLayer.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceLoaders.h"
#include "debug_draw.h"
#include "brofiler.h"
#include "JobSystem/JobSystem.h"
#include "Rendering/RenderResourceHelper.h"
#include "EntitySystem/World.h"
#include "EntitySystem/EntityTable.h"
#include "EntitySystem/ComponentTable.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/PhysicsWorld.h"
#include "EntitySystem/RigidBody/RigidbodySystem.h"

#define USE_OPENGL_GFX
#include "Core/GDI/inc_gfx.h"


namespace Dawn
{
	SEventHandle g_MouseMovedEvtHandle, g_MousePressedHandle, g_MouseReleasedHandle;
	uint64_t Application::FrameCount = 0;

	Application::Application(AppSettings& InSettings)
	{
		this->Settings = InSettings;
		Window = std::make_unique<Dawn::Window>();
		Locator::Add(AppLocatorId, this);
	}

	Application::~Application()
	{
		Locator::Remove(AppLocatorId);
	}

	void Application::Run()
	{
		// Resource System initialization
		if (!ResourceSystem.Initialize("E:/Git/engine/Dawn/Assets/", { ".obj", ".jpg", ".png", ".shader", ".PNG", ".fbx" }))
		{
			DWN_CORE_ERROR("Couldn't initialize resource system");
			system("pause");
			return;
		}

		ResourceSystem.AddRef();
		ResourceSystem.RegisterLoader(ResourceType_Model, BIND_FS_LOADER(Dawn::RS_LoadModel));
		ResourceSystem.RegisterLoader(ResourceType_Shader, BIND_FS_LOADER(Dawn::RS_LoadShader));
		ResourceSystem.RegisterLoader(ResourceType_Image, BIND_FS_LOADER(Dawn::RS_LoadImage));

		if (!ResourceSystem.BuildDatabase())
		{
			DWN_CORE_ERROR("Couldn't build database for resource system");
			system("pause");
			return;
		}

		// Window Creation Setup
		EResult Result = this->Window->Initialize(Settings.Title, Settings.Width,
								Settings.Height, Settings.IsFullscreen, 
								Settings.ColorBits, Settings.DepthBits, 
								Settings.AlphaBits);

		this->Window->OnWindowPaint = std::bind(&Application::Tick, this); 

		if (Result != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize window");
			system("pause");
			return;
		}
		
		if (Window->Create() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't create window system");
			system("pause");
			return;
		}
		Settings.Hwnd = Window->GetHwnd();

		GfxGDI::Create();
		if (!GfxGDI::Get()->Init(Settings))
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


		Physics = std::make_unique<PhysicsWorld>();
		if(!Physics->Initialize())
		{
			DWN_CORE_ERROR("Couldn't initialize physx: {0} !\n", PX_PHYSICS_VERSION);
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
		Input::Reset();

		while (true)
		{
			if (!Window->PeekMessages())
				break;
		}

		ClearLayers();
		Physics->Shutdown();
		GfxGDI::Get()->Shutdown();
		ResourceSystem.Shutdown();
		JobSystem::Shutdown();

		DWN_CORE_INFO("Core Context shutdown.");
	}

	Camera* g_Camera;
	

	void Application::Load()
	{
		RenderResourceHelper::LoadCommonShaders();

		// Boot up World!
		World = std::make_unique<Dawn::World>();
		World->AddTable("Transform", std::make_unique<ComponentTable<Transform>>());
		World->AddTable("Camera", std::make_unique<ComponentTable<Camera>>());
		World->AddSystem(std::make_unique<RigidbodySystem>());
		
		g_Camera = CreateCamera("Cam0", 
									Settings.Width, 
									Settings.Height, 
									0.1f, 10000.0f, 65.0f, 
									vec4(0.4f, 0.6f, 0.9f, 1.0f),
									vec3(0, 3, 10)
								  );
		
		CameraUtils::CalculatePerspective(g_Camera);

		auto Cam1 = CreateCamera("Cam1",
					Settings.Width,
					Settings.Height,
					0.0f, 10000.0f, 65.0f,
					vec4(0.4f, 0.6f, 0.9f, 1.0f),
					vec3(0, 0, 0),
					quat(1, 0, 0, 0)
				);

		CameraUtils::CalculateOthographic(Cam1);

		GfxImmediatePrimitives::Get()->AllocateBuffers();
	}

	void Application::Tick()
	{BROFILER_FRAME("MainThread")
		
		if (!IsInitialized)
			return;

		++Application::FrameCount;
		
		Clock.Tick();

		auto GDI = GfxGDI::Get();

		GDI->SetViewport(0, 0, Settings.Width, Settings.Height);
		GDI->SetClearColor(g_Camera->ClearColor);
		GDI->Clear();

		for (Layer* layer : Layers)
		{
			layer->Process();
		}

		GDI->Present();
		Input::Reset();
	}

	void Application::SetupLayers()
	{
		LayerInsertCount = Layers.begin();

		this->PushLayer(new ImGuiLayer(Window->GetHwnd()));
		this->PushLayer(new TestRenderLayer());
		this->PushLayer(new WorldSimulateLayer());

		for (Layer* layer : Layers)
			layer->Setup();
	}

	void Application::PushLayer(Layer* InLayer)
	{
		LayerInsertCount = Layers.emplace(LayerInsertCount, InLayer);
	}

	void Application::PopLayer(Layer* InLayer)
	{
		auto it = std::find(begin(), end(), InLayer);
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
