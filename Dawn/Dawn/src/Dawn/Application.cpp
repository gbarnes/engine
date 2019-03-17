#include "Application.h"
#include "imgui.h"
#include "Core/GDI/GfxBackend.h"
#include "Core/GDI/GfxQueue.h"
#include "Core/GDI/GfxDevice.h"
#include "Core/GDI/inc_gfx_types.h"
#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "inc_common.h"
#include "Layers/ImGuiLayer.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "Layers/TestRenderLayer.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include <d3d12.h>
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

	Application::Application(SAppSettings& InSettings)
	{
		this->Settings = InSettings;
		CLocator::Add(AppLocatorId, this);
	}

	Application::~Application()
	{
		CLocator::Remove(AppLocatorId);
	}

	void Application::OnMouseMovedEvent(CEvent& InEvent)
	{
		CMouseMovedEvent& e = static_cast<CMouseMovedEvent&>(InEvent);
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());
	}

	void Application::OnMousePressedEvent(CEvent& InEvent)
	{
		ImGuiIO& io = ImGui::GetIO();
		CMousePressedEvent& e = static_cast<CMousePressedEvent&>(InEvent);
		io.MouseDown[e.GetButton()] = true;
	}

	void Application::OnMouseReleasedEvent(CEvent& InEvent)
	{
		ImGuiIO& io = ImGui::GetIO();
		CMouseReleasedEvent& e = static_cast<CMouseReleasedEvent&>(InEvent);
		io.MouseDown[e.GetButton()] = false;
	}


	void Application::Run()
	{
		EResult Result = this->Window.Initialize(Settings.Title, Settings.WindowWidth,
								Settings.WindowHeight, Settings.IsFullscreen, 32, 32, 8);

		if (Result != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize window");
			system("pause");
			return;
		}

		this->Window.OnWindowPaint = std::bind(&Application::Tick, this);
		
		// BEGIN Subscriptions
		g_MouseMovedEvtHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseMoved"), BIND_EVENT_MEMBER(Application::OnMouseMovedEvent));
		g_MousePressedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MousePressed"), BIND_EVENT_MEMBER(Application::OnMousePressedEvent));
		g_MouseReleasedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseReleased"), BIND_EVENT_MEMBER(Application::OnMouseReleasedEvent));
		// END Subscriptions

		if (Window.Create() != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't create window system");
			system("pause");
			return;
		}

		if (GfxBackend::Initialize(Settings.WindowWidth, Settings.WindowHeight, Window.GetHwnd(), false, false) != EResult_OK)
		{
			DWN_CORE_ERROR("Couldn't initialize graphics device!\n");
			system("pause");
			return;
		}

		SetupLayers();

		DWN_CORE_INFO("Core Context initialized.\n");

		while (true)
		{
			if (Window.PeekMessages())
				break;
		}

		ClearLayers();
		GfxBackend::Shutdown();

		DWN_CORE_INFO("Core Context shutdown.\n");
	}

	void Application::Load()
	{
	}

	void Application::Tick()
	{
		if (!GfxBackend::IsInitialized())
			return;

		for (CLayer* layer : Layers)
		{
			layer->Update();
		}

		auto rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		auto dsv = GfxBackend::GetDepthBufferDescHandle();
		auto CmdList = GfxBackend::GetQueue()->GetCommandList();

		{
			GfxBackend::TransitionResource(CmdList, GfxBackend::GetCurrentBackbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			GfxBackend::ClearRenderTarget(CmdList, GfxBackend::GetCurrentBackbuffer(), g_ClearColor);
			GfxBackend::ClearDepthBuffer(CmdList, 1);
			CmdList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		}

		for (CLayer* layer : Layers)
		{
			layer->Render(CmdList);
		}
		
		GfxBackend::Present(CmdList);
	}

	void Application::SetupLayers()
	{
		LayerInsertCount = Layers.begin();
		//this->PushLayer(new CTestRenderLayer());
		this->PushLayer(new CImGuiLayer(Window.GetHwnd()));

		for (CLayer* layer : Layers)
			layer->Setup();
	}

	void Application::PushLayer(CLayer* InLayer)
	{
		LayerInsertCount = Layers.emplace(LayerInsertCount, InLayer);
	}

	void Application::PopLayer(CLayer* InLayer)
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
		for (CLayer* layer : Layers) 
		{
			layer->Free();
			delete layer;
		}

		Layers.clear();
	}
}
