#include "ImGuiLayer.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "imgui.h"
#include "Application.h"

namespace Dawn
{

	ImGuiLayer::ImGuiLayer(const Shared<Dawn::Application>& InApplication, HWND InWindowHandle)
		: WindowHandle(InWindowHandle), Layer(InApplication)
	{
		this->Application = InApplication;
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