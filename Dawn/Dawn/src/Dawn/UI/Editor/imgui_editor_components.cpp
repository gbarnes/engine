#include "imgui_editor_components.h"
#include "imgui_editor_functions.h"
#include "imgui.h"
#include "EntitySystem/Entity.h"
#include "EntitySystem/Camera/Camera.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Lights/LightComponents.h"

namespace Dawn
{
	void ShowEntity(Entity* InEntity)
	{
		if (InEntity == nullptr)
			return;

		if (ImGui::CollapsingHeader("Entity"))
		{
			auto meta = EntityTable::GetMeta(*InEntity);

			ImGui::Indent(10.0f);
			ImGui::Text("Id: %u", InEntity->Id);
			ImGui::Text("Name: %s", meta->Name.c_str());
			ImGui::Checkbox("Is Active", &meta->bIsActive);
			ImGui::Unindent(10.0f);
		}
	}

	void ShowTransformComponent(Transform* InTransform, ImGuizmo::MODE& InEditSpace)
	{
		if (InTransform == nullptr)
			return;

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

			ImGui::InputFloat3("Position", &InTransform->Position[0]);
			ImGui::InputFloat3("Scale", &InTransform->Scale[0]);
			ImGui::InputFloat4("Rotation", &InTransform->Rotation[0]);

			ImGui::Unindent(10.0f);
		}
	}

	void ShowCameraComponent(Camera* InCamera)
	{
		
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
				bHasChanged = true;

			if (ImGui::SliderFloat("FarZ", &InCamera->FarZ, 100.0f, 10000.0f))
				bHasChanged = true;

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
		if (ImGui::CollapsingHeader("DirectionalLight"))
		{
			ImGui::Indent(10.0f);
			ImGui::ColorEdit4("Color", &InLight->Color[0]);
			ImGui::InputFloat("Intensity", &InLight->Intensity, 0.1f, 200.0f, "%.4f", 0.1f);
			ImGui::Checkbox("Cast Shadows", &InLight->bCastShadows);
			ImGui::Unindent(10.0f);
		}
	}

	void ShowPointLightComponent(PointLight* InLight)
	{
		if (ImGui::CollapsingHeader("PointLight"))
		{
			ImGui::Indent(10.0f);
			ImGui::ColorEdit4("Color", &InLight->Color[0]);
			ImGui::InputFloat("Intensity", &InLight->Intensity, 0.1f, 1000.0f);
			ImGui::InputFloat("Range", &InLight->Range, 0.1f, 1000.0f);
			ImGui::Unindent(10.0f);
		}
	}

}