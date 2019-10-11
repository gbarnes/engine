#include "imgui_editor_functions.h"
#include "imgui.h"
#include "inc_common.h"
#include "UI/UIEditorEvents.h"
#include "imgui_editor_components.h"
#include "EntitySystem/Entity.h"
#include "EntitySystem/EntityTable.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Camera/Camera.h"
#include "ResourceSystem/Resources.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"

namespace Dawn 
{
	Shared<ResourceSystem> g_ResourceSystem = nullptr;
	std::vector<FileMetaData> g_MetaData;
	bool g_showAssetBrowser = false;
	void ShowAssetBrowserWindow()
	{
		if(g_ResourceSystem == nullptr)
			g_ResourceSystem = g_Application->GetResourceSystem();

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

	Entity* g_SelectedEntity = nullptr;
	bool g_ShowPropertyWindow = false;
	void ShowPropertyWindow()
	{
		ImGui::Begin(g_SelectedEntity->Name.c_str(), &g_ShowPropertyWindow);

		ShowEntity(g_SelectedEntity);

		auto world = g_Application->GetWorld(); 
		auto components = world->GetComponentTypesByEntity(g_SelectedEntity->Id);
		for (auto component : components)
		{
			if (component == "Camera")
				ShowCameraComponent(world->GetComponentByEntity<Camera>(g_SelectedEntity->Id));
			else if(component == "Transform")
				ShowTransformComponent(world->GetComponentByEntity<Transform>(g_SelectedEntity->Id));
			else if (component == "DirectionalLight")
				ShowDirectionalLightComponent(world->GetComponentByEntity<DirectionalLight>(g_SelectedEntity->Id));
		}

		ImGui::End();
	}

	bool g_ShowSceneWindow = false;
	void ShowSceneWindow()
	{
		auto world = g_Application->GetWorld();
		auto transforms = world->GetComponentsByType<Transform>();

		ImGui::Begin("Scene", &g_ShowSceneWindow);
		if (ImGui::TreeNode("Root"))
		{
			for (auto transform : transforms)
			{
				auto entity = transform->GetEntity();

				ImGui::Text(entity->Name.c_str());
				if (ImGui::IsItemClicked())
				{
					g_ShowPropertyWindow = true;
					g_SelectedEntity = const_cast<Entity*>(entity);
				}
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
				//if (ImGui::MenuItem("Camera")) {g_ShowCameraEditWindow = !g_ShowCameraEditWindow;}
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

		if (g_ShowPropertyWindow)
			ShowPropertyWindow();

		if (g_showAssetBrowser)
			ShowAssetBrowserWindow();

		if (g_ShowSceneWindow)
			ShowSceneWindow();
	}


}