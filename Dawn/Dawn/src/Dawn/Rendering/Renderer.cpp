#include "Renderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"

namespace Dawn
{
	void DeferredRenderer::AllocateTransientData(Application* InApp)
	{
		auto GDI = InApp->GetGDI();
		auto Settings = InApp->GetSettings();

		GfxRenderBuffer* Buffer;
		auto IdBuffer = GDI->CreateRenderBuffer(&Buffer);
		if (IdBuffer.IsValid)
		{
			Buffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
			Buffer->AttachColorTarget(1, Settings->Width, Settings->Height);	// Normal
			Buffer->AttachColorTarget(2, Settings->Width, Settings->Height);	// Albedo, AO
			Buffer->AttachColorTarget(3, Settings->Width, Settings->Height);	// Metallic,Roughness
			Buffer->AttachDepthStencilTarget(Settings->Width, Settings->Height);
			TransientData.RenderBufferId = IdBuffer;
		}

		GfxRenderBuffer* ScreenBuffer;
		IdBuffer = GDI->CreateRenderBuffer(&ScreenBuffer);
		if (IdBuffer.IsValid)
		{
			ScreenBuffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
			TransientData.FinalBufferId = IdBuffer;
		}
	}

	void DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
	{
		if (auto Buffer = InGDI->GetRenderBuffer(TransientData.RenderBufferId))
		{
			Buffer->AttachColorTarget(0, InWidth, InHeight);	// Position
			Buffer->AttachColorTarget(1, InWidth, InHeight);	// Normal
			Buffer->AttachColorTarget(2, InWidth, InHeight);	// Albedo
			Buffer->AttachColorTarget(3, InWidth, InHeight);	// Albedo
			Buffer->AttachDepthStencilTarget(InWidth, InHeight);
		}

		if (auto Buffer2 = InGDI->GetRenderBuffer(TransientData.FinalBufferId))
		{
			Buffer2->AttachColorTarget(0, InWidth, InHeight);	// Position
			Buffer2->AttachDepthStencilTarget(InWidth, InHeight);
		}
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, World* InWorld)
	{

		glDisable(GL_BLEND);

		// todo --- change the way how we obtain the camera ... for now we just use the default one!
		auto Camera = InWorld->GetCamera(0);
		PerFrameData.GeometryBucket.Reset(2048, TransientData.RenderBufferId, Camera->GetView(), Camera->GetProjection());
		PerFrameData.LightingBucket.Reset(4, TransientData.FinalBufferId, mat4(), mat4());
		PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());
	}

	void DeferredRenderer::Submit(Application* InApp)
	{
		// todo --- do this in parallel 
		PerFrameData.GeometryBucket.Sort();
		PerFrameData.LightingBucket.Sort();
		PerFrameData.FinalPassBucket.Sort();
		// SortTask.Join();
		PerFrameData.GeometryBucket.Submit(InApp);
		PerFrameData.LightingBucket.Submit(InApp);
		PerFrameData.FinalPassBucket.Submit(InApp);
	}

	void DeferredRenderer::EndFrame(GfxGDI* InGDI)
	{
		// todo --- move this somewhere else in dedicated render calls?!
		// Render IMGUI!
		{
			ImGuiWrapper::BeginNewFrame();
			RenderEditorUI();
			ImGuiWrapper::Render();
		}

		InGDI->Present();

		PerFrameData.FinalPassBucket.Free();
		PerFrameData.LightingBucket.Free();
		PerFrameData.GeometryBucket.Free();
	}
}