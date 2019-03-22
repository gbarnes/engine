#include "Application.h"
#include "imgui.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
#include "Core/GDI/GfxDevice.h"
#include "Core/GDI/GfxCmdList.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "inc_common.h"
#include "Layers/ImGuiLayer.h"
#include "Layers/TestRenderLayer.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceLoaders.h"

//#include "Core/JobSystem/JobSystem.h"

/*
void empty_job2(Dawn::SJob* InJob)
{
	cout2_mutex.lock();
	std::cout << "Hello world - Thread Index: " << Dawn::CJobSystem::ThreadIndex << "\n";
	cout2_mutex.unlock();
}

/*void empty_job(SJob* InJob)
{
cout2_mutex.lock();
int i = 12 + 12;
std::cout << "#" << x <<": 12 + 12 = " << i << ", Thread Index: " << CJobSystem::ThreadIndex << "\n";

OutputDebugString(L"");

x++;
cout2_mutex.unlock();

/*SJob* newJob = CJobSystem::CreateJob(&empty_job2);
CJobSystem::Run(newJob);
CJobSystem::Wait(newJob);
}*/


/*CJobSystem::Initialize();

	unsigned int ThreadIndex = CJobSystem::ThreadIndex;

	SJob* Root = CJobSystem::CreateJob(&empty_job2);
	for (unsigned int i = 0; i < 500; ++i)
	{
		SJob* Job = CJobSystem::CreateJobAsChild(Root, &empty_job2);
		CJobSystem::Run(Job);
	}

	CJobSystem::Run(Root);
	CJobSystem::Wait(Root);
	
	//RenderCommand::Packet p = RenderCommand::Create<Draw>(sizeof(Draw));
	//Draw* d = RenderCommand::GetCommand<Draw>(p);
	*/



namespace Dawn
{
	DirectX::XMFLOAT4 g_ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };
	SEventHandle g_MouseMovedEvtHandle, g_MousePressedHandle, g_MouseReleasedHandle;
	uint64_t Application::FrameCount = 0;

	Application::Application(SAppSettings& InSettings)
	{
		this->Settings = InSettings;
		Locator::Add(AppLocatorId, this);
	}

	Application::~Application()
	{
		Locator::Remove(AppLocatorId);
	}

	void Application::OnMouseMovedEvent(Event& InEvent)
	{
		/*MouseMovedEvent& e = static_cast<MouseMovedEvent&>(InEvent);
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());*/
	}

	void Application::OnMousePressedEvent(Event& InEvent)
	{
		/*ImGuiIO& io = ImGui::GetIO();
		MousePressedEvent& e = static_cast<MousePressedEvent&>(InEvent);
		io.MouseDown[e.GetButton()] = true;*/
	}

	void Application::OnMouseReleasedEvent(Event& InEvent)
	{
		/*ImGuiIO& io = ImGui::GetIO();
		MouseReleasedEvent& e = static_cast<MouseReleasedEvent&>(InEvent);
		io.MouseDown[e.GetButton()] = false;*/
	}


	void Application::Run()
	{
		// BEGIN Subscriptions
		g_MouseMovedEvtHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseMoved"), BIND_EVENT_MEMBER(Application::OnMouseMovedEvent));
		g_MousePressedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MousePressed"), BIND_EVENT_MEMBER(Application::OnMousePressedEvent));
		g_MouseReleasedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseReleased"), BIND_EVENT_MEMBER(Application::OnMouseReleasedEvent));
		// END Subscriptions

		// Resource System initialization
		if (!ResourceSystem.Initialize("../Assets/", { ".obj", ".jpg", ".cso" }))
		{
			DWN_CORE_ERROR("Couldn't initialize resource system");
			system("pause");
			return;
		}

		ResourceSystem.AddRef();
		ResourceSystem.RegisterLoader(ResourceType_StaticMesh, BIND_FS_LOADER(Dawn::RS_LoadStaticMesh));
		ResourceSystem.RegisterLoader(ResourceType_Shader, BIND_FS_LOADER(Dawn::RS_LoadShader));
		ResourceSystem.RegisterLoader(ResourceType_Image, BIND_FS_LOADER(Dawn::RS_LoadImage));

		if (!ResourceSystem.BuildDatabase())
		{
			DWN_CORE_ERROR("Couldn't build database for resource system");
			system("pause");
			return;
		}

		// Window Creation Setup
		EResult Result = this->Window.Initialize(Settings.Title, Settings.WindowWidth,
								Settings.WindowHeight, Settings.IsFullscreen, 32, 32, 8);

		if (Result != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize window");
			system("pause");
			return;
		}

		Clock.Reset();
		this->Window.OnWindowPaint = std::bind(&Application::Tick, this);
	
		if (Window.Create() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't create window system");
			system("pause");
			return;
		}

		// Graphics System Setups.
		if (GfxBackend::Initialize(Settings.WindowWidth, Settings.WindowHeight, Window.GetHwnd(), false, false) != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize graphics device!\n");
			system("pause");
			return;
		}

		SetupLayers();

		DWN_CORE_INFO("Core Context initialized.");

		while (true)
		{
			if (Window.PeekMessages())
				break;
		}

		ResourceSystem.Shutdown();
		ClearLayers();
		GfxBackend::Shutdown();

		DWN_CORE_INFO("Core Context shutdown.");
	}

	void Application::Load()
	{
	}

	void Application::Tick()
	{
		if (!GfxBackend::IsInitialized())
			return;

		++Application::FrameCount;

		Clock.Tick();

		auto rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		auto dsv = GfxBackend::GetDepthBufferDescHandle();
		auto CmdList = GfxBackend::GetQueue()->GetCommandList();

		{
			GfxBackend::TransitionResource(CmdList->GetGraphicsCommandList(), GfxBackend::GetCurrentBackbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			GfxBackend::ClearRenderTarget(CmdList->GetGraphicsCommandList(), GfxBackend::GetCurrentBackbuffer(), g_ClearColor);
			GfxBackend::ClearDepthBuffer(CmdList->GetGraphicsCommandList(), 1);
			CmdList->GetGraphicsCommandList()->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		}

		for (Layer* layer : Layers)
		{
			layer->Update();
			layer->Render(CmdList->GetGraphicsCommandList());
		}
		
		GfxBackend::Present(CmdList);
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
