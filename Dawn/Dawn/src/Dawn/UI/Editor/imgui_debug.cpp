#include "stdafx.h"
#include "imgui_debug.h"
#include "Core/GDI/GfxResource.h"
#include "Rendering/Renderer.h"
#include "Application.h"
#include "imgui.h"

namespace Dawn
{
	void ShowGBuffer(GfxRenderBuffer* InBuffer, GfxRenderBuffer* InShadowBuffer, GfxRenderBuffer* InSSAOBuffer)
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

		ImGui::Begin("SSAO", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, 100));
		ImGui::Image(InSSAOBuffer->GetColorTarget(0), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();
	}

	void ShowFpsCounter()
	{
		u32 drawCalls = g_Application->GetRenderer()->Stats.DrawCalls;
		u64 textureMemory = g_Application->GetRenderer()->Stats.TextureMemory;

		ImGui::SetNextWindowBgAlpha(0.3f);
		ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
			ImGuiWindowFlags_NoNav);
		ImGui::SetWindowPos(ImVec2(10, 30));
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "Fps: %.1f", ImGui::GetIO().Framerate);
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Frame: %.3f ms", ImGui::GetIO().DeltaTime);
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Draw Calls: %i", drawCalls);
		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Texture Mem: %.2f mb", ((double)textureMemory / 1024.0 / 1024.0));
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