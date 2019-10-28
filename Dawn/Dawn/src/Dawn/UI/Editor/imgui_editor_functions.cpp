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
#include "Rendering/Renderer.h"
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
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.3f ms/frame (%.1f FPS)", ImGui::GetIO().DeltaTime, ImGui::GetIO().Framerate);
	//	g_Application->GetDeltaTime();
		ImGui::End();
	}

	Entity* g_SelectedEntity = nullptr;
	bool g_ShowPropertyWindow = false;
	void ShowPropertyWindow()
	{
		ImGui::Begin("Inspector", &g_ShowPropertyWindow);

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
			else if (component == "PointLight")
				ShowPointLightComponent(world->GetComponentByEntity<PointLight>(g_SelectedEntity->Id));
		}

		ImGui::End();
	}

	bool g_ShowSceneWindow = false;
	void ShowSceneWindow()
	{
		auto world = g_Application->GetWorld();
		auto transforms = world->GetComponentsByType<Transform>();
		ImGui::SetNextWindowBgAlpha(1.f);
		ImGui::Begin("Scene", &g_ShowSceneWindow);
		
		if (ImGui::TreeNode("Root"))
		{
			for (auto transform : transforms)
			{
				auto entity = transform->GetEntity();
				if (entity->bIsHiddenInEditorHierarchy)
					continue;

				ImGui::Text(entity->Name.c_str());
				if (ImGui::IsItemClicked())
				{
					g_ShowPropertyWindow = true;
					g_SelectedEntity = entity;
				}
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}


	bool g_showMaterialBrowserWindow = false;
	void ShowMaterialBrowserWindow()
	{
		auto resources = g_Application->GetResourceSystem();
		static Material* SelectedMaterial = nullptr;

		ImGui::Begin("Materials", &g_showMaterialBrowserWindow);

		if (ImGui::TreeNode("Root"))
		{
			auto  materials = resources->GetMaterials();
			for (auto material : materials)
			{
				ImGui::Text(material->Name.c_str());
				if (ImGui::IsItemClicked())
				{
					SelectedMaterial = material;
				}
			}
			ImGui::TreePop();
		}

		if (SelectedMaterial)
		{
			ImGui::BeginChild("Inspector");
			
			ImGui::ColorEdit4("Color", &SelectedMaterial->Albedo[0]);
			ImGui::SliderFloat("Metallic", &SelectedMaterial->Metallic, 0.0f, 1.0f);
			ImGui::SliderFloat("Roughness", &SelectedMaterial->Roughness, 0.0f, 1.0f);

			ImGui::EndChild();
		}

		ImGui::End();
		//resources->GetAllMaterials();
	}

	void Dawn::RenderEditorUI()
	{
		/*static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		static bool bOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &bOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}*/

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
				if (ImGui::MenuItem("Materials")) { g_showMaterialBrowserWindow = !g_showMaterialBrowserWindow; }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Debug"))
			{
				static bool FpsCheckBoxVariable = false;
				if (ImGui::Checkbox("FPS", &FpsCheckBoxVariable)) { g_ShowFpsCounter = !g_ShowFpsCounter; }
				ImGui::Separator();
				//if (ImGui::MenuItem("Profiler")) {}
				//if (ImGui::MenuItem("Console")) {}

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Rendering"))
			{
				auto Renderer = g_Application->GetRenderer();
				// note--- this is only temporary here!
				ImGui::Text("SSAO");
				ImGui::Checkbox("Active", &Renderer->SSAOSettings.bIsActive);
				ImGui::InputFloat("Radius", &Renderer->SSAOSettings.Radius);
				ImGui::InputFloat("Bias", &Renderer->SSAOSettings.Bias);
				ImGui::InputFloat("Power", &Renderer->SSAOSettings.Power);
				ImGui::Separator();
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

		if (g_showMaterialBrowserWindow)
			ShowMaterialBrowserWindow();

	

		//ImGui::End();

	}



}