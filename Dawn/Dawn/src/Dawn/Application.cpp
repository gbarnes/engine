#include "Application.h"

#include "Core/GDI/GfxBackend.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "Core/Event.h"
#include "Core/Events/MouseEvent.h"
#include "imgui.h"
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
		// Todo: change this since initialize could fail and leave the application in a potentially bad state!
		this->Window.Initialize(InSettings.Title, InSettings.WindowWidth, InSettings.WindowHeight, InSettings.IsFullscreen, 32, 32, 8);
		this->Window.OnWindowPaint = std::bind(&Application::Tick, this);
		CLocator::Add(std::string("App"), this);

		g_MouseMovedEvtHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseMoved"), BIND_EVENT_MEMBER(Application::OnMouseMovedEvent));
		g_MousePressedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MousePressed"), BIND_EVENT_MEMBER(Application::OnMousePressedEvent));
		g_MouseReleasedHandle = CEventDispatcher::Subscribe(EVENT_KEY("MouseReleased"), BIND_EVENT_MEMBER(Application::OnMouseReleasedEvent));
	}

	Application::~Application()
	{
		CLocator::Remove(std::string("App"));
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
		io.MouseDown[0] = true;
	}

	void Application::OnMouseReleasedEvent(CEvent& InEvent)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[0] = false;
	}

	void Application::Run()
	{

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

		ImGuiWrapper::Create(Window.GetHwnd());
		
		/*if (CJobSystem::Initialize() != EResult_OK)
		{
			std::cout << "Couldn't initialize job system!\n";
			system("pause");
			return;
		}*/

		DWN_CORE_INFO("Core Context initialized.\n");


		while (true)
		{
			if (Window.PeekMessages())
				break;
		}

		ImGuiWrapper::Shutdown();

		//CJobSystem::Shutdown();
		GfxBackend::Shutdown();

		DWN_CORE_INFO("Core Context shutdown.\n");
	}

	void Application::Tick()
	{
		if (!GfxBackend::IsInitialized())
			return;

		ImGuiWrapper::BeginNewFrame();

		GfxBackend::Reset();
		GfxBackend::ClearRenderTarget(GfxBackend::GetCurrentBackbuffer(), g_ClearColor);
		ImGuiWrapper::Render();
		GfxBackend::Present();
	}
}
