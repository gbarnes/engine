#include "imgui_editor_functions.h"
#include "imgui.h"
#include "inc_common.h"
#include "UI/UIEditorEvents.h"

#include "EntitySystem/Entity.h"
#include "EntitySystem/EntityTable.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Camera/Camera.h"
#include "ResourceSystem/Resources.h"
#include "ResourceSystem/ResourceSystem.h"

namespace Dawn 
{
	void ShowEntity(const Entity* InEntity)
	{
		auto transform = const_cast<Entity*>(InEntity)->GetTransform();

		auto type = Transform::GetType()->Get<vec3>(transform, "Position");

		if (ImGui::CollapsingHeader("Entity"))
		{
			ImGui::Indent(10.0f);
			ImGui::Text("Id: %u", InEntity->Id.Index);
			ImGui::Text("Name: %s", InEntity->Name.c_str());
			ImGui::Unindent(10.0f);
		}

		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::Indent(10.0f);
			ImGui::InputFloat3("Position", &transform->Position[0]);
			ImGui::InputFloat3("Scale", &transform->Scale[0]);
			ImGui::InputFloat4("Rotation", &transform->Rotation[0]);
			ImGui::Unindent(10.0f);
		}
	}

	ResourceSystem* g_ResourceSystem = nullptr;
	std::vector<FileMetaData> g_MetaData;
	bool g_showAssetBrowser = false;
	void ShowAssetBrowserWindow()
	{
		if(g_ResourceSystem == nullptr)
			g_ResourceSystem = ResourceSystem::Get();

		if(g_MetaData.size() == 0)
			g_MetaData = g_ResourceSystem->GetAllMetaFiles();

		ImGui::Begin("Asset Browser", &g_showAssetBrowser);
		

		// Render table header!
		{
			ImGui::BeginGroup();
			ImGui::Columns(4, "mycolumns3", true);
			ImGui::Text("Id");
			ImGui::NextColumn();
			ImGui::Text("Name");
			ImGui::NextColumn();
			ImGui::Text("Path");
			ImGui::NextColumn();
			ImGui::Text("Filesize");
			ImGui::NextColumn();
			ImGui::EndGroup();

			for (auto metaFile : g_MetaData)
			{
				ImGui::BeginGroup();
				ImGui::Columns(4, "mycolumns3", true);
				ImGui::Text("%llu", metaFile.Id);
				ImGui::NextColumn();
				ImGui::Text(metaFile.Name.c_str());
				ImGui::NextColumn();
				ImGui::Text(metaFile.Path.c_str());
				ImGui::NextColumn();
				ImGui::Text("%.2f mb", ((double)metaFile.Size / 1024.0 / 1024.0));
				ImGui::NextColumn();
				ImGui::EndGroup();
			}
		}
		
		ImGui::End();
	}

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
		auto camera = GetCamera(0);
		auto transform = camera->GetTransform();
		auto entity = camera->GetEntity();
	
		ImGui::Begin(entity->Name.c_str(), &g_ShowCameraEditWindow);

		ShowEntity(entity);

		if (ImGui::CollapsingHeader("Camera"))
		{
			ImGui::Indent(10.0f);

			auto forward = TransformUtils::CalculateForward(transform);
			auto up = TransformUtils::CalculateUp(transform);
			auto right = TransformUtils::CalculateRight(transform);

			ImGui::Text("Forward %.2f %.2f %.2f", forward.x, forward.y, forward.z);
			ImGui::Text("Up %.2f %.2f %.2f", up.x, up.y, up.z);
			ImGui::Text("Right %.2f %.2f %.2f", right.x, right.y, right.z);
			ImGui::Spacing();

			ImGui::InputFloat3("World Up", &camera->WorldUp[0]);

			ImGui::ColorEdit4("Clear Color", &camera->ClearColor[0]);

			if (ImGui::SliderFloat("Field Of View", &camera->FieldOfView, 12.0f, 120.0f))
				CameraUtils::CalculatePerspective(camera);

			if (ImGui::SliderFloat("Aspect Ratio", &camera->AspectRatio, 0.0, 20.0f))
				CameraUtils::CalculatePerspective(camera);

			if (ImGui::SliderFloat("NearZ", &camera->NearZ, 0.01f, 2.0f))
				CameraUtils::CalculatePerspective(camera);

			if (ImGui::SliderFloat("FarZ", &camera->FarZ, 100.0f, 10000.0f))
				CameraUtils::CalculatePerspective(camera);

			ImGui::Unindent(10.0f);
		}
		
		ImGui::End();
	}

	bool g_ShowSceneWindow = false;
	void ShowSceneWindow()
	{
		ImGui::Begin("Scene", &g_ShowSceneWindow);

		
		if (ImGui::TreeNode("Root"))
		{
			/*if (ImGui::TreeNode("Editor Camera"))
			{
				ImGui::TreePop();
			}*/

			if (ImGui::Button("Editor Camera"))
			{

			}

			ImGui::TreePop();
		}

		ImGui::End();
	}


	void Dawn::RenderEditorUI()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("World"))
			{
				if (ImGui::MenuItem("Scene")) { g_ShowSceneWindow = !g_ShowSceneWindow; }
				if (ImGui::MenuItem("Camera")) {g_ShowCameraEditWindow = !g_ShowCameraEditWindow;}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Assets"))
			{
				if (ImGui::MenuItem("Browser")) { g_showAssetBrowser = !g_showAssetBrowser; }
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

		if (g_showAssetBrowser)
			ShowAssetBrowserWindow();

		if (g_ShowSceneWindow)
			ShowSceneWindow();
	}


}