#include "stdafx.h"
#include "imgui_editor_utils.h"
#include "imgui_editor_types.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Transform/Transform.h"

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

			auto* Transform = InWorld->GetComponentByEntity<Dawn::Transform>(SceneData->SelectionData.Entity);
			auto* ParentTransform = Transform->GetParent(InWorld);
			mat4& ParentWorldSpace = (ParentTransform) ? ParentTransform->WorldSpace : Transform->WorldSpace;
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

			if (SceneData->EditSpace != ImGuizmo::LOCAL)
				Transform->Position = (ParentTransform) ? (Selection.Position + vec3(-ParentWorldSpace[3][0], -ParentWorldSpace[3][1], -ParentWorldSpace[3][2])) : Selection.Position;

			Transform->Scale = (ParentTransform) ? Selection.Scale * vec3(1.0f / ParentWorldSpace[0][0], 1.0f / ParentWorldSpace[1][1], 1.0f / ParentWorldSpace[2][2]) : Selection.Scale;
			Transform->Rotation = glm::quat(glm::vec3(glm::radians(Selection.Rotation.x), glm::radians(Selection.Rotation.y), glm::radians(Selection.Rotation.z)));
		}
	}
}