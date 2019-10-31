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
#include "vendor/ImGuizmo/ImGuizmo.h"
#include "imgui_editor_types.h"
#include "Core/Input.h"
#include "imgui_editor_gizmo.h"

namespace Dawn 
{
	Shared<ResourceSystem> g_ResourceSystem = nullptr;
	std::vector<FileMetaData> g_MetaData;

	void Editor_LoadResources()
	{
		auto Resources = Editor_GetResources();

		if (!Resources->bIsLoaded)
		{
			auto ResourceSystem = g_Application->GetResourceSystem();
			auto GDI = g_Application->GetGDI();

			auto handle = ResourceSystem->LoadFile("Textures/editor/lightbulb.png");
			if (handle.IsValid)
			{
				Resources->Icons[EI_ScenePointLightIcon] = ResourceSystem->FindImage(handle)->TextureId;
			}

			handle = ResourceSystem->LoadFile("Textures/editor/lightbulb-directional.png");
			if (handle.IsValid)
			{
				Resources->Icons[EI_SceneDirLightIcon] = ResourceSystem->FindImage(handle)->TextureId;
			}

			handle = ResourceSystem->LoadFile("Textures/editor/camera.png");
			if (handle.IsValid)
			{
				Resources->Icons[EI_SceneCameraIcon] = ResourceSystem->FindImage(handle)->TextureId;
			}

			
			Resources->bIsLoaded = true;
		}
	}

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

	
	bool g_ShowPropertyWindow = false;
	void ShowPropertyWindow()
	{
		ImGui::Begin("Inspector", &g_ShowPropertyWindow);

		auto SceneData = Editor_GetSceneData();
		auto SelectedEntity = SceneData->CurrentSelectedEntity;

		ShowEntity(SelectedEntity);

		auto world = g_Application->GetWorld(); 
		auto components = world->GetComponentTypesByEntity(SelectedEntity->Id);
		for (auto component : components)
		{
			if (component == "Camera")
				ShowCameraComponent(world->GetComponentByEntity<Camera>(SelectedEntity->Id));
			else if(component == "Transform")
				ShowTransformComponent(world->GetComponentByEntity<Transform>(SelectedEntity->Id), SceneData->EditSpace);
			else if (component == "DirectionalLight")
				ShowDirectionalLightComponent(world->GetComponentByEntity<DirectionalLight>(SelectedEntity->Id));
			else if (component == "PointLight")
				ShowPointLightComponent(world->GetComponentByEntity<PointLight>(SelectedEntity->Id));
		}

		ImGui::End();
	}

	bool g_ShowSceneWindow = false;
	void ShowSceneWindow()
	{
		auto world = g_Application->GetWorld();
		auto transforms = world->GetComponentsByType<Transform>();
		auto SceneData = Editor_GetSceneData();

		ImGui::SetNextWindowBgAlpha(1.f);
		ImGui::Begin("Scene", &g_ShowSceneWindow);
		
		if (ImGui::TreeNode("Root"))
		{
			for (auto transform : transforms)
			{
				auto entity = transform->GetEntity();
				if (entity->bIsHiddenInEditorHierarchy)
					continue;

				int selected = (entity == SceneData->CurrentSelectedEntity) ? ImGuiTreeNodeFlags_Selected : 0;

				if (ImGui::TreeNodeEx(entity->Name.c_str(), selected | ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemClicked())
					{
						g_ShowPropertyWindow = true;
						SceneData->CurrentSelectedEntity = entity;
					}

					ImGui::TreePop();
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

	void Dawn::Editor_RenderUI()
	{
		auto World = g_Application->GetWorld();
		// todo --- implement another way of getting the camera. since not always the editor cam will be at id 0
		auto EditorCamera = World->GetCamera(0);
		auto GDI = g_Application->GetGDI();
		auto SceneData = Editor_GetSceneData();
		auto Resources = Editor_GetResources();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		Editor_LoadResources();

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
		
		if (g_ShowPropertyWindow)
			ShowPropertyWindow();

		if (g_showAssetBrowser)
			ShowAssetBrowserWindow();

		if (g_ShowSceneWindow)
			ShowSceneWindow();

		if (g_showMaterialBrowserWindow)
			ShowMaterialBrowserWindow();


		Editor_BeginGizmoFrame();
		Editor_RenderObjectManipulationGizmo(EditorCamera, World.get(), Editor_GetSceneData());
		Editor_RenderDirectionalLightGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
		Editor_RenderPointLightGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
		Editor_RenderCameraGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
	}



}