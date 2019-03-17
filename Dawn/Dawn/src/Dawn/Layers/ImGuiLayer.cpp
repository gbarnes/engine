#include "ImGuiLayer.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "imgui.h"

namespace Dawn
{
	CImGuiLayer::CImGuiLayer(HWND InWindowHandle)
		: WindowHandle(InWindowHandle)
	{
	}

	CImGuiLayer::~CImGuiLayer()
	{
	}

	void CImGuiLayer::Setup()
	{
		ImGuiWrapper::Create(&WindowHandle);
	}

	void CImGuiLayer::Update()
	{
		ImGuiWrapper::BeginNewFrame();
		ConstructFpsDisplay();
	}

	void CImGuiLayer::Render(ComPtr<CGfxCmdList> InCmdList)
	{
		// TODO: passing a cmd list has to change since 
		//		 we might want to use multiple cmd list for different
		//	     layers or something.
		ImGuiWrapper::Render(InCmdList.Get());
	}

	void CImGuiLayer::Free()
	{
		ImGuiWrapper::Shutdown();
	}

	void CImGuiLayer::ConstructFpsDisplay()
	{
		bool isOpen = false;
		ImGui::Begin("Stats", &isOpen, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs);
		ImGui::SetWindowPos(ImVec2(20, 20));
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}