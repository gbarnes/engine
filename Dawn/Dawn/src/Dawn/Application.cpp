#include "Application.h"
#include "imgui.h"

#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "inc_common.h"
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
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/PhysicsWorld.h"
#include "EntitySystem/RigidBody/RigidbodySystem.h"
#include "Core/Config.h"
#include "Rendering/Renderer.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "UI/Editor/imgui_debug.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "Vendor/ImGuizmo/ImGuizmo.h"

#define USE_OPENGL_GFX
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	Shared<Application> g_Application = nullptr;
	SEventHandle g_MouseMovedEvtHandle, g_MousePressedHandle, g_MouseReleasedHandle;

	AppSettings* Application::GetSettings()
	{
		assert(g_Application != nullptr);
		return &g_Application->Settings;
	}

	Application::Application(AppSettings& InSettings)
	{
		this->Settings = InSettings;
		Window = std::make_unique<Dawn::Window>();
	}

	Application::~Application()
	{
	}

	static bool bIsInEditMode = false;
	static bool bShowGBuffer = false;
	static bool bShowFPS = false;

	void OnPostRender(GfxGDI* InGDI, DeferredRenderer* InRenderer)
	{
		ImGuiWrapper::BeginNewFrame();
		ImGuizmo::BeginFrame();

		if (bShowFPS)
			ShowFpsCounter();

		if(bShowGBuffer)
			ShowGBuffer(InGDI->GetRenderBuffer(InRenderer->TransientData.GBufferId), InGDI->GetRenderBuffer(InRenderer->TransientData.ShadowMapBufferId));


		if (bIsInEditMode)
			Editor_RenderUI();

		ImGuiWrapper::Render();
	}
	

	void Application::Run()
	{
		Config::Load({ "Engine" });

		// Load Engine related config values and apply them
		Config::GetBool("Engine", "Debug/ShowFps", &bShowFPS);

		//Memory::HeapArea Heap(_128MB);
		//SimpleArena Arena(Heap);
		
		//AppSettings* Settings2 = D_NEW(AppSettings, Arena);
		//Settings2->AlphaBits = 0;

		ResourceSystem = Dawn::ResourceSystem::Create(Paths::ProjectContentDir(), { ".obj", ".jpg", ".png", ".shader", ".PNG", ".fbx" }, true);
		ResourceSystem->RegisterLoader(BIND_FS_LOADER(Dawn::RS_LoadModel), BIND_FS_LOADER(Dawn::RS_ReloadModel), { ".obj", ".fbx" });
		ResourceSystem->RegisterLoader(BIND_FS_LOADER(Dawn::RS_LoadShader), BIND_FS_LOADER(Dawn::RS_LoadShader), { ".shader" });
		ResourceSystem->RegisterLoader(BIND_FS_LOADER(Dawn::RS_LoadImage), BIND_FS_LOADER(Dawn::RS_ReloadImage), { ".jpg", ".png", ".PNG" });

		if (!ResourceSystem->BuildDatabase())
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
		if (Result != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize window");
			system("pause");
			return;
		}

		this->Window->OnWindowResize = std::bind(&Application::Resize, this, std::placeholders::_1, std::placeholders::_2);
		
		if (Window->Create() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't create window system");
			system("pause");
			return;
		}
		Settings.Hwnd = Window->GetHwnd();
		
		this->GDI = std::shared_ptr<GfxGDI>(GfxGDI::Create());
		if (!this->GDI->Init(Settings))
		{
			DWN_CORE_ERROR("Couldn't initialize GDI!\n");
			system("pause");
			return;
		}

		Renderer = std::make_shared<DeferredRenderer>();
		Renderer->AllocateTransientData(this);
		Renderer->OnPostRender = std::bind(&OnPostRender, std::placeholders::_1, std::placeholders::_2);

		JobSystem::Initialize();

		// Boot up World!
		World = std::make_unique<Dawn::World>();
		
		World->AddTable("Transform", std::make_unique<ComponentTable<Transform>>());
		World->AddTable("Camera", std::make_unique<ComponentTable<Camera>>());
		World->AddTable("DirectionalLight", std::make_unique<ComponentTable<DirectionalLight>>());
		World->AddTable("PointLight", std::make_unique<ComponentTable<PointLight>>());
		//World->AddSystem(std::make_unique<RigidbodySystem>());

		Physics = std::make_unique<PhysicsWorld>();
		if(!Physics->Initialize())
		{
			DWN_CORE_ERROR("Couldn't initialize physx: {0} !\n", PX_PHYSICS_VERSION);
			system("pause");
			return;
		}

		{
			InitInput(Window->GetInstance(), Window->GetHwnd(), Settings.Width, Settings.Height);
			Load();
			SetupLayers();
		}

		ImGuizmo::SetOrthographic(false);

		DWN_CORE_INFO("Core Context initialized.");

		{
			// todo --- move this into a custom clock/timer class!
			// since we only target windows we can use the performance functions.
			InitTime(Time);
			IsInitialized = true;
			bool bExit = false;
		
			while (!bExit)
			{BROFILER_FRAME("MainThread");
			
				MSG msg;
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT) bExit = true;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				Tick();
			}
		}

		// -------- CLEANUP
		{
			Cleanup();
			ClearLayers();
			Physics->Shutdown();
			JobSystem::Shutdown();
			GDI->Shutdown();
			ResourceSystem->Shutdown();
			ShutdownInput();

			DWN_CORE_INFO("Core Context shutdown.");
		}
	}

	void Application::Tick()
	{
		UpdateInput();
		Time.FrameCount++;
		
#if DAWN_DEBUG
		if (IsKeyPressed(KeyCode::KeyCode_F6))
			bIsInEditMode = !bIsInEditMode;

		if (IsKeyPressed(KeyCode::KeyCode_F5))
			bShowGBuffer = !bShowGBuffer;
#endif
		
		{BROFILER_EVENT("UPDATE")

			Time.AlignedUpdateDeltaTime += Time.FrameDeltaTime;
			while (Time.AlignedUpdateDeltaTime >= Time::TargetUpdateRate)
			{
				Update(Time.AlignedUpdateDeltaTime * Time.TimeScale); 
				Time.AlignedUpdateDeltaTime -= Time::TargetUpdateRate;
			}
		}

		{BROFILER_EVENT("FIXED UPDATE")
			
			Time.AlignedPhysicsDeltaTime += Time.FrameDeltaTime;
			while (Time.AlignedPhysicsDeltaTime >= Time::TargetPhysicsUpdateRate)
			{
				const float FixedTime = Time.AlignedPhysicsDeltaTime * Time.TimeScale;
				FixedUpdate(FixedTime);

				// Take care of updating the physics engine!
				auto scene = GetPhysics()->GetScene();
				if (scene)
				{
					scene->simulate(FixedTime);
					scene->fetchResults(true);
				
				}

				ResourceSystem->Refresh();
				Time.AlignedPhysicsDeltaTime -= Time::TargetPhysicsUpdateRate;
			}
		}

		StepTime(Time);

		// Rendering
		{BROFILER_EVENT("Rendering")

			Renderer->BeginFrame(GDI.get(), World.get());

			for (auto Layer : Layers)
				Layer->Render();

			Renderer->Submit(this);
			Renderer->EndFrame(GDI.get());
		}
	}

	void Application::Render()
	{
	}

	void Application::Resize(int width, int height)
	{
		if (width == 0 || height == 0)
			return;

		auto Cameras = World->GetCameras();
		for (auto Camera : Cameras)
		{
			Camera->Width = width;
			Camera->Height = height;
			Camera->AspectRatio = (float)width / (float)height;

			if(Camera->bIsOrthographic)
				CameraUtils::CalculateOthographic(Camera);
			else 
				CameraUtils::CalculatePerspective(Camera);
		}

		Settings.Width = width;
		Settings.Height = height;
		Renderer->Resize(GDI.get(), width, height);
		DWN_CORE_INFO("Resizing Window to {0}x{1}!", width, height);
	}

	void Application::SetupLayers()
	{
		LayerInsertCount = Layers.begin();

		//this->PushLayer(new ImGuiLayer(this->shared_from_this(), Window->GetHwnd()));
		this->PushLayer(new TestRenderLayer(this->shared_from_this()));
		this->PushLayer(new WorldSimulateLayer(this->shared_from_this()));

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