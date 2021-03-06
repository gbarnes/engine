#include "stdafx.h"
#include "imgui_editor_gizmo.h"
#include "imgui.h"
#include "Application.h"
#include "Core/Input.h"
#include "EntitySystem/Entity.h"
#include "Vendor/ImGuizmo/ImGuizmo.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Camera/Camera.h"
#include "Core/GDI/GfxGDI.h"
#include "imgui_editor_types.h"
#include "imgui_debug.h"
#include <Brofiler.h>

namespace Dawn
{
	void Editor_BeginGizmoFrame()
	{
		auto SceneData = Editor_GetSceneData();
		ImGuiIO& io = ImGui::GetIO();

		const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
		ImGui::PushStyleColor(ImGuiCol_Border, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("icons", NULL, flags);
		SceneData->GizmoDrawList = ImGui::GetWindowDrawList();
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);
	}


	void Editor_RenderObjectManipulationGizmo(Camera* InEditorCamera, World* InWorld, EditorSceneData* InSceneData)
	{
		if (InSceneData->SelectionData.Entity.IsValid())
		{
			auto meta = InWorld->GetEntityMetaData(InSceneData->SelectionData.Entity);
			if (meta->bIsHiddenInEditorHierarchy)
				return;

			const mat4& view = InEditorCamera->GetView();
			const mat4& proj = InEditorCamera->GetProjection();

			ImGuizmo::Enable(true);

			ImGuizmo::Manipulate(&view[0][0], &proj[0][0], InSceneData->EditMethod, InSceneData->EditSpace, &InSceneData->SelectionData.ModelMatrix[0][0]);
			
			if (!IsMouseDown(MouseBtn_Right))
			{
				if (IsKeyPressed(KeyCode::KeyCode_W))
					InSceneData->EditMethod = ImGuizmo::OPERATION::TRANSLATE;

				if (IsKeyPressed(KeyCode::KeyCode_E))
					InSceneData->EditMethod = ImGuizmo::OPERATION::SCALE;

				if (IsKeyPressed(KeyCode::KeyCode_R))
					InSceneData->EditMethod = ImGuizmo::OPERATION::ROTATE;
			}
		}
	}

	void Editor_RenderPointLightGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld, 
		EditorSceneData* InSceneData, EditorResources* InResources)
	{
		BROFILER_EVENT("Editor_PointLightGizmos");

		auto Texture = InGDI->GetTexture(InResources->Icons[EI_ScenePointLightIcon]);
		auto PointLightList = InWorld->GetComponentSets<PointLight, Transform>();

		ImVec2 size = ImVec2(64, 64);

		for (auto PointLight : PointLightList)
		{	
			auto meta = InWorld->GetEntityMetaData(PointLight.first->GetEntity());
			if (meta->bIsHiddenInEditorHierarchy)
				continue;

			if (!CameraUtils::IsPointVisible(InEditorCamera->GetFrustum(), PointLight.second->Position))
				continue;

			vec2 ScreenPos = CameraUtils::WorldToScreenPoint(InEditorCamera, PointLight.second->Position);

			ImVec2 pos[2] =
			{
				ImVec2(ScreenPos.x - size.x * 0.5f, ScreenPos.y - size.y * 0.5f),
				ImVec2(ScreenPos.x + size.x * 0.5f, ScreenPos.y + size.y * 0.5f)
			};

			ImVec4 Color = ImVec4(PointLight.first->Color.r, PointLight.first->Color.g, PointLight.first->Color.b, PointLight.first->Color.a);
			auto ConvertedColor = ImGui::ColorConvertFloat4ToU32(Color);

			if (ImGui::IsMouseHoveringRect(pos[0], pos[1], false) && ImGui::IsMouseClicked(0))
			{
				InSceneData->SelectionData.Entity = PointLight.first->GetEntity();
				InSceneData->SelectionData.ModelMatrix = PointLight.second->WorldSpace;
			}

			InSceneData->GizmoDrawList->AddImage(Texture->GetGPUAddress(), pos[0], pos[1], ImVec2(0, 0), ImVec2(1, 1), ConvertedColor);
		}
	}

	void Editor_RenderDirectionalLightGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld, 
		EditorSceneData* InSceneData, EditorResources* InResources)
	{
		BROFILER_EVENT("Editor_DirLightGizmos");

		auto LightBulbDirTexture = InGDI->GetTexture(InResources->Icons[EI_SceneDirLightIcon]);
		ImVec2 size = ImVec2(64, 64);

		auto DirLightList = InWorld->GetComponentSets<DirectionalLight, Transform>();
		for (auto DirLight : DirLightList)
		{
			auto meta = InWorld->GetEntityMetaData(DirLight.first->GetEntity());
			if (meta->bIsHiddenInEditorHierarchy)
				continue;

			if (!CameraUtils::IsPointVisible(InEditorCamera->GetFrustum(), DirLight.second->Position))
				continue;

			vec2 ScreenPos = CameraUtils::WorldToScreenPoint(InEditorCamera, DirLight.second->Position);
			
			ImVec2 pos[2] =
			{
				ImVec2(ScreenPos.x - size.x * 0.5f, ScreenPos.y - size.y * 0.5f),
				ImVec2(ScreenPos.x + size.x * 0.5f, ScreenPos.y + size.y * 0.5f)
			};

			ImVec4 Color = ImVec4(DirLight.first->Color.r, DirLight.first->Color.g, DirLight.first->Color.b, DirLight.first->Color.a);
			auto ConvertedColor = ImGui::ColorConvertFloat4ToU32(Color);

			if (ImGui::IsMouseHoveringRect(pos[0], pos[1], false) && ImGui::IsMouseClicked(0))
			{
				InSceneData->SelectionData.Entity = DirLight.first->GetEntity();
				InSceneData->SelectionData.ModelMatrix = DirLight.second->WorldSpace;
			}

			InSceneData->GizmoDrawList->AddImage(LightBulbDirTexture->GetGPUAddress(), pos[0], pos[1], 
				ImVec2(0, 0), ImVec2(1, 1), ConvertedColor);
		}
	}

	void Editor_RenderCameraGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld,
		EditorSceneData* InSceneData, EditorResources* InResources)
	{
		BROFILER_EVENT("Editor_CameraGizmos");

		auto Texture = InGDI->GetTexture(InResources->Icons[EI_SceneCameraIcon]);
		ImVec2 size = ImVec2(64, 47);

		auto CameraList = InWorld->GetComponentSets<Camera, Transform>();
		for (auto Camera : CameraList)
		{
			auto meta = InWorld->GetEntityMetaData(Camera.first->GetEntity());
			if (meta->bIsHiddenInEditorHierarchy)
				continue;

			if (!CameraUtils::IsPointVisible(InEditorCamera->GetFrustum(), Camera.second->Position))
				continue;

			vec2 ScreenPos = CameraUtils::WorldToScreenPoint(InEditorCamera, Camera.second->Position);
			
			ImVec2 pos[2] =
			{
				ImVec2(ScreenPos.x - size.x * 0.5f, ScreenPos.y - size.y * 0.5f),
				ImVec2(ScreenPos.x + size.x * 0.5f, ScreenPos.y + size.y * 0.5f)
			};

			if (ImGui::IsMouseHoveringRect(pos[0], pos[1], false) && ImGui::IsMouseClicked(0))
			{
				InSceneData->SelectionData.Entity = Camera.first->GetEntity();
				InSceneData->SelectionData.ModelMatrix = Camera.second->WorldSpace;
			}

			if (InSceneData->SelectionData.Entity.Id == Camera.first->GetEntity().Id)
			{
				vec2 ForwardPos = CameraUtils::WorldToScreenPoint(InEditorCamera, Camera.second->Position - Camera.second->Forward * 5.0f);
				InSceneData->GizmoDrawList->AddLine(ImVec2(ScreenPos.x, ScreenPos.y), ImVec2(ForwardPos.x, ForwardPos.y), 0xff0000ff, 5.0f);
			}

			InSceneData->GizmoDrawList->AddImage(Texture->GetGPUAddress(), pos[0], pos[1],
				ImVec2(0, 0), ImVec2(1, 1), 0xffffffff);
		}
	}

}