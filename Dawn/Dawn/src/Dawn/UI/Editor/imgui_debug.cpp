#include "stdafx.h"
#include "imgui_debug.h"
#include "Core/GDI/Base/GfxResource.h"
#include "Rendering/DeferredRenderer.h"
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
		//ImGui::Image(InBuffer->GetColorTarget(0), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0,0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Normal", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		
		ImGui::SetWindowPos(ImVec2((float)Settings->Width / 4, yOffset));
		//ImGui::Image(InBuffer->GetColorTarget(1), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("GBuffer_Albedo", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(((float)Settings->Width / 4) * 2, yOffset));
		//ImGui::Image(InBuffer->GetColorTarget(2), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();


		ImGui::Begin("Shadowmap", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(((float)Settings->Width / 4) * 3, yOffset));
		//ImGui::Image(InShadowBuffer->GetDepthTarget(), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
		ImGui::End();

		ImGui::Begin("SSAO", 0, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);
		ImGui::SetWindowPos(ImVec2(0, 100));
		//ImGui::Image(InSSAOBuffer->GetColorTarget(0), ImVec2((float)Settings->Width / 4, (float)Settings->Height / 4), ImVec2(0, 0), ImVec2(1, -1));
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

	
	constexpr float FramgraphTimeLimit = 40.0f; // Limit at 40ms
	constexpr float FramgraphHeight = 300.0f;
	constexpr float SixtyFpsPaddingMark = FramgraphHeight * (((100.0f / FramgraphTimeLimit)  *  15.5555555f) / 100.0f);
	constexpr float ThirtyFpsPaddingMark = FramgraphHeight * (((100.0f / FramgraphTimeLimit) * 32.22222222f) / 100.0f);
	constexpr const char* PlotLabel = "Frametimes";
	constexpr i32 AmountFrames = 300;
	float gFrames[AmountFrames];
	i32 Frames;

	void ShowFramegraph()
	{
		const auto time = g_Application->GetTime();
		float fps = time.FrameDeltaTime * 1000;//ImGui::GetIO().DeltaTime * 1000;

		for (size_t i = 1; i < AmountFrames; ++i)
		{
			gFrames[i - 1] = gFrames[i];
		}

		gFrames[AmountFrames - 1] = std::max(fps, 16.66f);
		
		auto Settings = g_Application->GetSettings();

		ImGui::Begin(PlotLabel, nullptr, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);

		ImGui::SetWindowSize(ImVec2(Settings->Width, FramgraphHeight));
		ImGui::SetWindowPos(ImVec2(0.0f, Settings->Height - FramgraphHeight));
		
		ImGui::PlotLines(PlotLabel, &gFrames[0], AmountFrames, 0, PlotLabel, 0.0f, 40.00f, ImVec2(Settings->Width, FramgraphHeight));
		
		auto* drawlist = ImGui::GetWindowDrawList();
		drawlist->AddText(ImVec2(15.0f, Settings->Height - SixtyFpsPaddingMark - 15.0f), IM_COL32(114, 165, 49, 255), "16.66ms");
		drawlist->AddLine(ImVec2(9.0f, Settings->Height - SixtyFpsPaddingMark), ImVec2(Settings->Width, Settings->Height - SixtyFpsPaddingMark), IM_COL32(114, 165, 49, 255), 1.0f);
		
		drawlist->AddText(ImVec2(15.0f, Settings->Height - ThirtyFpsPaddingMark - 15.0f), IM_COL32(252, 102, 5, 255), "33.33ms");
		drawlist->AddLine(ImVec2(9.0f, Settings->Height - ThirtyFpsPaddingMark), ImVec2(Settings->Width, Settings->Height - ThirtyFpsPaddingMark), IM_COL32(252, 102, 5, 255), 1.0f);

		ImGui::End();
	}
}