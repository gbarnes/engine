#include "imgui_editor_functions.h"
#include "imgui.h"
#include "UI/UIEditorEvents.h"

namespace Dawn 
{
	bool g_ShowFpsCounter = false;
	void ShowFpsCounter()
	{
		ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoInputs);
		ImGui::SetWindowPos(ImVec2(10, 30));
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	bool g_ShowCameraEditWindow = false;
	void ShowCameraEditWindow()
	{
		static float fov = 45.0f;
		static float camPos[3] = { 0, 0, -10 };
		
		ImGui::Begin("Camera Properties", &g_ShowCameraEditWindow);
		if(ImGui::SliderFloat("Field Of View", &fov, 12.0f, 120.0f))
		{
			CEventDispatcher::Trigger(FOVChangedEvtKey, UIFovChangedEvent(fov));
		}
		if (ImGui::InputFloat3("Position", camPos)) 
		{
			CEventDispatcher::Trigger(CamPosChangedEvtKey, UICamPosChangedEvent(camPos));
		}
		ImGui::End();
	}


	void Dawn::RenderEditorUI()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				if (ImGui::MenuItem("Camera Properties")) {g_ShowCameraEditWindow = !g_ShowCameraEditWindow;}
				ImGui::EndMenu();
			}

			/*if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}*/

			
			if (ImGui::BeginMenu("Debug"))
			{
				static bool FpsCheckBoxVariable = false;
				if (ImGui::Checkbox("FPS", &FpsCheckBoxVariable)) { g_ShowFpsCounter = !g_ShowFpsCounter; }
				ImGui::Separator();
				if (ImGui::MenuItem("Profiler")) {}
				if (ImGui::MenuItem("Console")) {}
				
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (g_ShowFpsCounter)
			ShowFpsCounter();

		if (g_ShowCameraEditWindow)
			ShowCameraEditWindow();
	}


}