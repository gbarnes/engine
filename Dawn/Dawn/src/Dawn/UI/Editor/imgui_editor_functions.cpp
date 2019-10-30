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
#include "Core/Input.h"

namespace Dawn 
{
	struct EditorImageData
	{
		GfxResId LightbulbIcon;
		bool bIsLoaded = false;
	};

	
	ImGuizmo::MODE g_ObjectEditSpace = ImGuizmo::WORLD;
	ImGuizmo::OPERATION g_ObjectEditMode = ImGuizmo::TRANSLATE;
	EditorImageData g_ImageData;

	Shared<ResourceSystem> g_ResourceSystem = nullptr;
	std::vector<FileMetaData> g_MetaData;

	void LoadEditorResources()
	{
		if (!g_ImageData.bIsLoaded)
		{
			auto ResourceSystem = g_Application->GetResourceSystem();
			auto GDI = g_Application->GetGDI();

			auto handle = ResourceSystem->LoadFile("Textures/editor/lightbulb.png");
			if (handle.IsValid)
			{
				g_ImageData.LightbulbIcon = ResourceSystem->FindImage(handle)->TextureId;
			}

			g_ImageData.bIsLoaded = true;
		}
	}

	void RenderEntityIcons()
	{
		BROFILER_EVENT("Editor_Icons");

		auto World = g_Application->GetWorld();
		auto GDI = g_Application->GetGDI();
		auto Camera = World->GetCamera(0);
		auto LightBulbTexture = GDI->GetTexture(g_ImageData.LightbulbIcon);

		ImDrawList* DrawList;

		ImGuiIO& io = ImGui::GetIO();

		const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
		ImGui::PushStyleColor(ImGuiCol_Border, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("icons", NULL, flags);
		DrawList = ImGui::GetWindowDrawList();
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);

		ImVec2 size = ImVec2(64, 64);

		auto DirLightList = World->GetComponentSets<DirectionalLight, Transform>();
		for (auto DirLight : DirLightList)
		{
			vec2 ScreenPos = CameraUtils::WorldToScreenPoint(Camera, DirLight.second->Position);
			
			ImVec2 pos[2] =
			{
				ImVec2(ScreenPos.x - size.x * 0.5f, ScreenPos.y + size.y * 0.5f),
				ImVec2(ScreenPos.x + size.x * 0.5f, ScreenPos.y - size.y * 0.5f)
			};

			ImVec4 Color = ImVec4(DirLight.first->Color.r, DirLight.first->Color.g, DirLight.first->Color.b, DirLight.first->Color.a);
			auto ConvertedColor = ImGui::ColorConvertFloat4ToU32(Color);

			DrawList->AddImage(LightBulbTexture->GetGPUAddress(), pos[0], pos[1], ImVec2(0,0), ImVec2(1,1), ConvertedColor);
			//DrawList->AddImageQuad(LightBulbTexture->GetGPUAddress(), pos[0], pos[1], pos[2], pos[4], ImVec2(0,0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1));
		}


		auto PointLightList = World->GetComponentSets<PointLight, Transform>();
		for (auto PointLight : PointLightList)
		{
			vec2 ScreenPos = CameraUtils::WorldToScreenPoint(Camera, PointLight.second->Position);

			ImVec2 pos[2] =
			{
				ImVec2(ScreenPos.x - size.x * 0.5f, ScreenPos.y + size.y * 0.5f),
				ImVec2(ScreenPos.x + size.x * 0.5f, ScreenPos.y - size.y * 0.5f)
			};

			ImVec4 Color = ImVec4(PointLight.first->Color.r, PointLight.first->Color.g, PointLight.first->Color.b, PointLight.first->Color.a);
			auto ConvertedColor = ImGui::ColorConvertFloat4ToU32(Color);

			DrawList->AddImage(LightBulbTexture->GetGPUAddress(), pos[0], pos[1], ImVec2(0, 0), ImVec2(1, -1), ConvertedColor);
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
				ShowTransformComponent(world->GetComponentByEntity<Transform>(g_SelectedEntity->Id), g_ObjectEditSpace);
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
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		LoadEditorResources();

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

		auto gdi = g_Application->GetGDI();
		auto world = g_Application->GetWorld();
		auto editorCamera = world->GetCamera(0);
		const mat4& view = editorCamera->GetView();
		const mat4& proj = editorCamera->GetProjection();

		static mat4 ModelMatrix(1);
		static vec3 LastEuler;

		if (g_SelectedEntity != nullptr && !g_SelectedEntity->bIsHiddenInEditorHierarchy)
		{
			ImGuizmo::Enable(true);

			auto transform = g_SelectedEntity->GetTransform(world.get());
			auto rotation = LastEuler;

			ImGuizmo::RecomposeMatrixFromComponents(&transform->Position[0], &rotation[0], &transform->Scale[0], &ModelMatrix[0][0]);
			ImGuizmo::Manipulate(&view[0][0], &proj[0][0], g_ObjectEditMode, g_ObjectEditSpace, &ModelMatrix[0][0]);
			ImGuizmo::DecomposeMatrixToComponents(&ModelMatrix[0][0], &transform->Position[0], &rotation[0], &transform->Scale[0]);

			if (LastEuler != rotation)
			{ 
				LastEuler = rotation;
				DWN_CORE_INFO("Rotation: {0}, {1}, {2}", LastEuler.y, LastEuler.x, LastEuler.z);
				transform->Rotation = glm::angleAxis(glm::radians(rotation.y), vec3(0, 1, 0)) * glm::angleAxis(glm::radians(rotation.z), vec3(0, 0, 1)) * glm::angleAxis(glm::radians(rotation.x), vec3(1, 0, 0));
			}

			if (!IsMouseDown(MouseBtn_Right) && ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			{
				if (IsKeyPressed(KeyCode::KeyCode_W))
				{
					g_ObjectEditMode = ImGuizmo::OPERATION::TRANSLATE;
				}

				if (IsKeyPressed(KeyCode::KeyCode_E))
				{
					g_ObjectEditMode = ImGuizmo::OPERATION::SCALE;
				}

				if (IsKeyPressed(KeyCode::KeyCode_R))
				{
					g_ObjectEditMode = ImGuizmo::OPERATION::ROTATE;
				}
			}
		}

		RenderEntityIcons();
	}



}