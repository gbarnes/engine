#include "stdafx.h"
#include "imgui_debug.h"
#include "Core/GDI/GfxResource.h"
#include "Rendering/Renderer.h"
#include "Application.h"
#include "imgui.h"

namespace Dawn
{
	void ShowGBuffer(GfxRenderBuffer* InBuffer, GfxRenderBuffer* InShadowBuffer)
	{
		auto Settings = g_Application->GetSettings();
		auto Renderer = g_Application->GetRenderer();
		static float yOffset = 20.0f;

		ImGui::Begin("GBuffer_Position", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(0), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0,0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Normal", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2((float)Settings->Width / 4, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(1), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Albedo", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(((float)Settings->Width / 4) * 2, yOffset));
		ImGui::Image(InBuffer->GetColorTarget(2), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();


		ImGui::Begin("Shadowmap", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(((float)Settings->Width / 4) * 3, yOffset));
		ImGui::Image(InShadowBuffer->GetDepthTarget(), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
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

	constexpr u8 debugTextBufferSize = 10;
	static std::string debugTextBuffer[debugTextBufferSize];
	static u8 debugTextBufferPosition = 0;
	
	void OutputDebugText(const std::string& InText)
	{
		debugTextBuffer[debugTextBufferPosition] = InText;
		debugTextBufferPosition = (debugTextBufferPosition + 1) % debugTextBufferSize;
	}

	void ShowDebugText()
	{
		ImGui::Begin("DebugOutput", nullptr, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | 
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);

		ImGui::SetWindowSize(ImVec2(500, 500));
		ImGui::SetWindowPos(ImVec2(10, 50));
		ImGui::BeginChild("Scrolling");
		for (u8 i = 0; i < debugTextBufferSize; ++i)
		{
			std::string& s = debugTextBuffer[i];
			if(s.length() > 0)
				ImGui::Text(s.c_str());
		}
		ImGui::EndChild();
		ImGui::End();
	}
}