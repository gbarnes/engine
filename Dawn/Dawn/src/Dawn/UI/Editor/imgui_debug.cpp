#include "imgui_debug.h"
#include "Core/GDI/GfxResource.h"
#include "Rendering/Renderer.h"
#include "Application.h"
#include "imgui.h"

namespace Dawn
{
	void ShowGBuffer(GfxRenderBuffer* InBuffer)
	{
		auto Settings = g_Application->GetSettings();
		auto Renderer = g_Application->GetRenderer();
		static u32 yOffset = 20;

		ImGui::Begin("GBuffer_Position", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(0), ImVec2(Settings->Width / 4, Settings->Height / 4), ImVec2(0,0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Normal", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(Settings->Width / 4, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(1), ImVec2(Settings->Width / 4, Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Albedo", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2((Settings->Width / 4) * 2, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(2), ImVec2(Settings->Width / 4, Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();
	}

	void ShowFpsCounter()
	{
		ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(10, 30));
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.3f ms/frame (%.1f FPS)", ImGui::GetIO().DeltaTime, ImGui::GetIO().Framerate);
		//	g_Application->GetDeltaTime();
		ImGui::End();
	}
}