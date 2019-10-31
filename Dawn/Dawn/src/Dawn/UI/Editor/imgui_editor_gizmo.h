#pragma once


namespace Dawn
{
	class World;
	class GfxGDI;

	struct Camera;
	struct EditorResources;
	struct EditorSceneData;

	void Editor_BeginGizmoFrame();
	void Editor_RenderObjectManipulationGizmo(Camera* InEditorCamera, World* InWorld, EditorSceneData* InSceneData);
	void Editor_RenderPointLightGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld,
		EditorSceneData* InSceneData, EditorResources* InResources);
	void Editor_RenderDirectionalLightGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld,
		EditorSceneData* InSceneData, EditorResources* InResources);
	void Editor_RenderCameraGizmos(GfxGDI* InGDI, Camera* InEditorCamera, World* InWorld,
		EditorSceneData* InSceneData, EditorResources* InResources);
}