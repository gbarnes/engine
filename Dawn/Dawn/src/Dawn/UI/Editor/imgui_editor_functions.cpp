#include "stdafx.h"
#include "imgui_editor_functions.h"
#include "imgui.h"
#include "UI/UIEditorEvents.h"
#include "imgui_editor_components.h"
#include "EntitySystem/Entity.h"
#include "EntitySystem/EntityTable.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/StaticMesh/ModelView.h"
#include "ResourceSystem/Resources.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Rendering/Renderer.h"
#include "Application.h"
#include "vendor/ImGuizmo/ImGuizmo.h"
#include "imgui_editor_types.h"
#include "Core/Input.h"
#include "Core/Paths.h"
#include "imgui_editor_gizmo.h"
#include "imgui_editor_assets.h"
#include "imgui_debug.h"

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
	void ShowPropertyWindow(World* InWorld)
	{
		ImGui::Begin("Inspector", &g_ShowPropertyWindow);

		auto SceneData = Editor_GetSceneData();
		auto SelectedEntity = SceneData->CurrentSelectedEntity;

		if (SelectedEntity.IsValid())
		{
			ShowEntity(InWorld, &SelectedEntity);

			auto world = g_Application->GetWorld();
			auto components = world->GetComponentTypesByEntity(SelectedEntity);
			for (auto component : components)
			{
				if (component == "Camera")
					ShowCameraComponent(world->GetComponentByEntity<Camera>(SelectedEntity));
				else if (component == "Transform")
					ShowTransformComponent(world->GetComponentByEntity<Transform>(SelectedEntity), SceneData->EditSpace);
				else if (component == "DirectionalLight")
					ShowDirectionalLightComponent(world->GetComponentByEntity<DirectionalLight>(SelectedEntity));
				else if (component == "PointLight")
					ShowPointLightComponent(world->GetComponentByEntity<PointLight>(SelectedEntity));
				else if (component == "ModelView")
					ShowModelViewComponent(world->GetComponentByEntity<ModelView>(SelectedEntity), g_Application->GetResourceSystem().get());
			}
		}

		ImGui::End();
	}

	bool g_ShowSceneWindow = false;
	void ShowSceneWindow()
	{
		ImGui::SetNextWindowBgAlpha(1.f);
		ImGui::Begin("Scene", &g_ShowSceneWindow);
		
		if (ImGui::TreeNode("Root"))
		{
			auto world = g_Application->GetWorld();
			auto transforms = world->GetComponentsByType<Transform>();
			auto SceneData = Editor_GetSceneData();

			for (auto transform : transforms)
			{
				auto entity = transform->GetEntity();
				auto meta = world->GetEntityMetaData(entity);
				if (meta->bIsHiddenInEditorHierarchy)
					continue;

				int selected = 0;
				
				if(SceneData->CurrentSelectedEntity.IsValid())
					selected = (entity.Id == SceneData->CurrentSelectedEntity.Id) ? ImGuiTreeNodeFlags_Selected : 0;

				if (ImGui::TreeNodeEx(meta->Name.c_str(), selected | ImGuiTreeNodeFlags_Leaf))
				{
					if (ImGui::IsItemClicked())
					{
						g_ShowPropertyWindow = true;
						SceneData->CurrentSelectedEntity = entity;
						SceneData->ModelMatrix = mat4(1);
						SceneData->LastEulerRotation = glm::eulerAngles(world->GetComponentByEntity<Transform>(entity)->Rotation);
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
		auto EditorWorld = g_Application->GetEditorWorld();
		// todo --- implement another way of getting the camera. since not always the editor cam will be at id 0
		auto EditorCamera = g_Application->GetEditorWorld()->GetCamera(0);
		auto GDI = g_Application->GetGDI();
		auto SceneData = Editor_GetSceneData();
		auto ResourceSystem = g_Application->GetResourceSystem();

		SceneData->Name = "Test";

		auto Resources = Editor_GetResources();

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		Editor_LoadResources();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Level")) { //Editor_NewScene(); 
				}
				if (ImGui::MenuItem("Save Level")) { Editor_SaveLevel(Paths::ProjectContentDir().append("Scenes/"), SceneData, World.get(), ResourceSystem.get()); }
				if (ImGui::MenuItem("Load Level")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				//if (ImGui::MenuItem("Camera")) {g_ShowCameraEditWindow = !g_ShowCameraEditWindow;}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Entities"))
			{
				if (ImGui::BeginMenu("Primitives"))
				{
					if (ImGui::MenuItem("Cube")) {}
					if (ImGui::MenuItem("Sphere")) {}
					if (ImGui::MenuItem("Cylinder")) {}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Lights"))
				{
					if (ImGui::MenuItem("Directional Light")) {}
					if (ImGui::MenuItem("Point Light")) {}
					ImGui::EndMenu();
				}
				
				if (ImGui::MenuItem("Camera")) { }
				//if (ImGui::MenuItem("Camera")) {g_ShowCameraEditWindow = !g_ShowCameraEditWindow;}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Scene")) { g_ShowSceneWindow = !g_ShowSceneWindow; }
				if (ImGui::MenuItem("Browser")) { g_showAssetBrowser = !g_showAssetBrowser; }
				if (ImGui::MenuItem("Materials")) { g_showMaterialBrowserWindow = !g_showMaterialBrowserWindow; }
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Rendering"))
			{
				auto Renderer = g_Application->GetRenderer();

				auto Camera = World::GetActiveCamera();

				ImGui::Text(fmt::format("Camera: {0}", Camera->GetEntityMeta()->Name).c_str());

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
			ShowPropertyWindow(World.get());

		if (g_showAssetBrowser)
			ShowAssetBrowserWindow();

		if (g_ShowSceneWindow)
			ShowSceneWindow();

		if (g_showMaterialBrowserWindow)
			ShowMaterialBrowserWindow();

		// todo (gb): add a command with undo and redo stuff and so on!
		if (IsKeyDown(KeyCode::KeyCode_Delete) && SceneData->CurrentSelectedEntity.IsValid())
		{
			World->DestroyEntity(SceneData->CurrentSelectedEntity);
			SceneData->CurrentSelectedEntity = INVALID_ENTITY;
			
		}

		Editor_BeginGizmoFrame();
		Editor_RenderObjectManipulationGizmo(EditorCamera, World.get(), Editor_GetSceneData());
		Editor_RenderDirectionalLightGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
		Editor_RenderPointLightGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
		Editor_RenderCameraGizmos(GDI.get(), EditorCamera, World.get(), SceneData, Resources);
	}
}