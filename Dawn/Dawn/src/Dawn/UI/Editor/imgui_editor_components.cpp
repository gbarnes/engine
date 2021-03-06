#include "stdafx.h"
#include "imgui_editor_components.h"
#include "imgui_editor_functions.h"
#include "imgui.h"
#include "EntitySystem/Entity.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Model/MeshFilter.h"
#include "ResourceSystem/ResourceSystem.h"
#include "imgui_editor_types.h"
#include "imgui_debug.h"

namespace Dawn
{
	void ShowEntity(World* InWorld, Entity* InEntity)
	{
		D_ASSERT(InEntity, "Entity to inspect is null");

		if (ImGui::CollapsingHeader("Entity"))
		{
			auto meta = InWorld->GetEntityMetaData(*InEntity);

			ImGui::Indent(10.0f);
			ImGui::Text("Id: %u", InEntity->Id);
			ImGui::Text("Name: %s", meta->Name.c_str());
			ImGui::Checkbox("Is Active", &meta->bIsActive);
			ImGui::Unindent(10.0f);
		}
	}

	void ShowTransformComponent(World* InWorld, Transform* InTransform, ImGuizmo::MODE& InEditSpace)
	{
		D_ASSERT(InTransform, "Transform to inspect is null");
		auto* SceneData = Editor_GetSceneData();
		auto* ParentTransform = InTransform->GetParent(InWorld);
		mat4& ParentWorldSpace = (ParentTransform) ? ParentTransform->WorldSpace : InTransform->WorldSpace;

		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::Indent(10.0f);

			auto forward = TransformUtils::CalculateForward(InTransform);
			auto up = TransformUtils::CalculateUp(InTransform);
			auto right = TransformUtils::CalculateRight(InTransform);

			//ImGui::Text("Forward %.2f %.2f %.2f", forward.x, forward.y, forward.z);
			//ImGui::Text("Up %.2f %.2f %.2f", up.x, up.y, up.z);
			//ImGui::Text("Right %.2f %.2f %.2f", right.x, right.y, right.z);

			if (ImGui::RadioButton("Local", InEditSpace == ImGuizmo::LOCAL))
				InEditSpace = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", InEditSpace == ImGuizmo::WORLD))
				InEditSpace = ImGuizmo::WORLD;

			ImGui::Spacing();

			ImGui::InputFloat3("Position", &SceneData->SelectionData.Position[0]);
			ImGui::InputFloat3("Scale", &InTransform->Scale[0]);
			ImGui::InputFloat3("Rotation", &SceneData->SelectionData.Rotation[0]);
		
			ImGui::Unindent(10.0f);
		}
	}

	void ShowCameraComponent(Camera* InCamera)
	{
		D_ASSERT(InCamera, "Camera to inspect is null");

		if (ImGui::CollapsingHeader("Camera"))
		{
			bool bHasChanged = false;

			ImGui::Indent(10.0f);

			ImGui::InputFloat3("World Up", &InCamera->WorldUp[0]);

			ImGui::ColorEdit4("Clear Color", &InCamera->ClearColor[0]);

			if (ImGui::SliderFloat("Field Of View", &InCamera->FieldOfView, 12.0f, 120.0f))
				bHasChanged = true;

			if (ImGui::SliderFloat("Aspect Ratio", &InCamera->AspectRatio, 0.0, 20.0f))
				bHasChanged = true;

			if (ImGui::SliderFloat("NearZ", &InCamera->NearZ, 0.01f, 2.0f)) 
			{
				bHasChanged = true;
				
				if (InCamera->NearZ <= 0)
					InCamera->NearZ = 0.1f;
			}

			if (ImGui::SliderFloat("FarZ", &InCamera->FarZ, 100.0f, 10000.0f)) 
			{
				bHasChanged = true;
				
				if (InCamera->FarZ <= 0)
					InCamera->NearZ = 1000.0f;
			}

			if (bHasChanged)
			{
				if (InCamera->bIsOrthographic)
					CameraUtils::CalculateOthographic(InCamera);
				else if (InCamera->bIsOrthographic)
					CameraUtils::CalculatePerspective(InCamera);
			}

			ImGui::Unindent(10.0f);
		}
	}

	void ShowDirectionalLightComponent(DirectionalLight* InLight)
	{
		D_ASSERT(InLight, "Light to inspect is null");

		if (ImGui::CollapsingHeader("DirectionalLight"))
		{
			ImGui::Indent(10.0f);
			ImGui::ColorEdit4("Color", &InLight->Color[0]);
			ImGui::InputFloat("Intensity", &InLight->Intensity, 0.1f, 200.0f, "%.4f");
			ImGui::Checkbox("Cast Shadows", &InLight->bCastShadows);
			ImGui::Unindent(10.0f);
		}
	}

	void ShowPointLightComponent(PointLight* InLight)
	{
		D_ASSERT(InLight, "Light to inspect is null");

		if (ImGui::CollapsingHeader("PointLight"))
		{
			ImGui::Indent(10.0f);
			ImGui::ColorEdit4("Color", &InLight->Color[0]);
			ImGui::InputFloat("Intensity", &InLight->Intensity, 0.1f, 1000.0f);
			ImGui::InputFloat("Range", &InLight->Range, 0.1f, 1000.0f);
			ImGui::Unindent(10.0f);
		}
	}

	void ShowMeshFilterComponent(MeshFilter* InModelView, ResourceSystem* InResourceSystem)
	{
		D_ASSERT(InModelView, "Model to inspect is null");

		if (ImGui::CollapsingHeader("Model"))
		{
			auto* ModelMetaData = InResourceSystem->GetMetaDataFromHandle(InModelView->ModelFileId);

			ImGui::Indent(10.0f);
			ImGui::Text("File: %s", (ModelMetaData) ? ModelMetaData->Name.c_str() : "Not set");
			ImGui::Unindent(10.0f);
		}
	}
}