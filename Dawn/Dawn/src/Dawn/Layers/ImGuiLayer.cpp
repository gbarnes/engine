#include "ImGuiLayer.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
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
		ImGuiWrapper::Create(WindowHandle);
	}

	void CImGuiLayer::Update()
	{
		ImGuiWrapper::BeginNewFrame();
		RenderEditorUI();
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
}