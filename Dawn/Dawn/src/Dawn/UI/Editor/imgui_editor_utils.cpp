#include "stdafx.h"
#include "imgui_editor_utils.h"
#include "imgui_editor_types.h"
#include "EntitySystem/World.h"
#include "ResourceSystem/Resources.h"
#include "EntitySystem/Transform/Transform.h"
#include "imgui_debug.h"

namespace Dawn
{
	void Editor_ReadGizmoManipulation(World* InWorld)
	{
		auto* SceneData = Editor_GetSceneData();
		
		if (SceneData->SelectionData.Entity.IsValid())
		{
			SceneData->SelectionData.Position = vec3(0);
			SceneData->SelectionData.Scale = vec3(1);
			SceneData->SelectionData.Rotation = vec3(0);

			ImGuizmo::DecomposeMatrixToComponents(&SceneData->SelectionData.ModelMatrix[0][0], &SceneData->SelectionData.Position[0], 
				&SceneData->SelectionData.Rotation[0], &SceneData->SelectionData.Scale[0]);
		}
	}

	void Editor_WriteGizmoManipulation(World* InWorld)
	{
		auto* SceneData = Editor_GetSceneData();

		if (SceneData->SelectionData.Entity.IsValid())
		{
			auto& Selection = SceneData->SelectionData;
			auto* Transform = InWorld->GetComponentByEntity<Dawn::Transform>(Selection.Entity);
			auto* ParentTransform = Transform->GetParent(InWorld);
			mat4 ParentWorldSpace = (ParentTransform) ? ParentTransform->WorldSpace : Transform->WorldSpace;

			ImGuizmo::RecomposeMatrixFromComponents(&Selection.Position[0], &Selection.Rotation[0], &Selection.Scale[0], &Selection.ModelMatrix[0][0]);

			// we want to transfer the data into local / object space from worldspace!
			quat rotation = glm::quat(glm::vec3(glm::radians(Selection.Rotation.x), glm::radians(Selection.Rotation.y), glm::radians(Selection.Rotation.z)));
			if (ParentTransform) 
			{
				vec3 translation;
				vec3 rot;
				vec3 scale;
				ImGuizmo::DecomposeMatrixToComponents(&ParentWorldSpace[0][0], &translation[0], &rot[0], &scale[0]);
		
				OutputDebugText(fmt::format("Rotation parent: {0}, {1}, {2}", rot.x, rot.y, rot.z));

				//Selection.Rotation -= (-1.0f*rot);
				Selection.Scale *= 1.0f / scale;
				Selection.Position -= translation;

				rotation *= glm::inverse(glm::quat(glm::vec3(glm::radians(rot.x), glm::radians(rot.y), glm::radians(rot.z))));
				OutputDebugText(fmt::format("Rotation Local: {0}, {1}, {2}", Selection.Rotation.x, Selection.Rotation.y, Selection.Rotation.z));
			}


			Transform->Position = Selection.Position;
			Transform->Scale = Selection.Scale;
			Transform->Rotation = rotation;
		}
	}

	void Editor_ShowMaterial(Material* InMaterial)
	{
		ImGui::ColorEdit4("Color", &InMaterial->Albedo[0], ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
		ImGui::SliderFloat("Metallic", &InMaterial->Metallic, 0.0f, 1.0f);
		ImGui::SliderFloat("Roughness", &InMaterial->Roughness, 0.0f, 1.0f);
	}
}