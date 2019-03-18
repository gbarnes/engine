#include "ImGuiLayer.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "imgui.h"

namespace Dawn
{
	ImGuiLayer::ImGuiLayer(HWND InWindowHandle)
		: WindowHandle(InWindowHandle)
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::Setup()
	{
		ImGuiWrapper::Create(WindowHandle);
	}

	void ImGuiLayer::Update()
	{
		ImGuiWrapper::BeginNewFrame();
		RenderEditorUI();
	}

	void ImGuiLayer::Render(ComPtr<CGfxCmdList> InCmdList)
	{
		// TODO: passing a cmd list has to change since 
		//		 we might want to use multiple cmd list for different
		//	     layers or something.
		ImGuiWrapper::Render(InCmdList.Get());
	}

	void ImGuiLayer::Free()
	{
		ImGuiWrapper::Shutdown();
	}
}