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

	void ImGuiLayer::Render()
	{
		BROFILER_CATEGORY("ImGuiLayer_Render", Brofiler::Color::Azure)
		ImGuiWrapper::Render();
	}

	void ImGuiLayer::Process()
	{
		Update();
		Render();
	}

	void ImGuiLayer::Free()
	{
		ImGuiWrapper::Shutdown();
	}
}