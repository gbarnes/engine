#include "Application.h"

#include "Core/GDI/GfxBackend.h"
#include "imgui.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"
//#include "Core/JobSystem/JobSystem.h"

namespace Dawn
{
	DirectX::XMFLOAT4 g_ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };

	Application::Application(SAppSettings& InSettings)
	{
		
		this->Settings = InSettings;
		// Todo: change this since initialize could fail and leave the application in a potentially bad state!
		this->Window.Initialize(InSettings.Title, InSettings.WindowWidth, InSettings.WindowHeight, InSettings.IsFullscreen, 32, 32, 8);
		this->Window.OnWindowPaint = std::bind(&Application::Tick, this);

		CLocator::Add(std::string("App"), this);
	}

	Application::~Application()
	{
		CLocator::Remove(std::string("App"));
	}

	void Application::Run()
	{
		if (Window.Create() != EResult_OK)
		{
			DWN_ERROR("Couldn't create window system");
			system("pause");
			return;
		}

		if (GfxBackend::Initialize(Settings.WindowWidth, Settings.WindowHeight, Window.GetHwnd(), false, false) != EResult_OK)
		{
			std::cout << "Couldn't initialize graphics device!\n";
			system("pause");
			return;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO(); 
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplWin32_Init(this->Window.GetHwnd());
		GfxBackend::ImGuiDX12Init();

		/*if (CJobSystem::Initialize() != EResult_OK)
		{
			std::cout << "Couldn't initialize job system!\n";
			system("pause");
			return;
		}*/

		while (true)
		{
			if (Window.PeekMessages())
				break;
		}

		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		//CJobSystem::Shutdown();
		GfxBackend::Shutdown();
	}

	void Application::Tick()
	{
		if (!GfxBackend::IsInitialized())
			return;

		GfxBackend::Reset();
		GfxBackend::ClearBackbuffer(g_ClearColor);
		GfxBackend::Present();
	}
}
