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
			Buffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Albedo
			Buffer->AttachDepthStencilTarget(Settings->Width, Settings->Height);
			TransientData.RenderBufferId = IdBuffer;
		}
	}

	void DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
	{
		if (auto Buffer = InGDI->GetRenderBuffer(TransientData.RenderBufferId))
		{
			Buffer->AttachColorTarget(0, InWidth, InHeight);
			Buffer->AttachDepthStencilTarget(InWidth, InHeight);
		}
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, World* InWorld)
	{
		// todo --- change the way how we obtain the camera ... for now we just use the default one!
		auto Camera = InWorld->GetCamera(0);
		PerFrameData.GeometryBucket.Reset(2048, TransientData.RenderBufferId, Camera->GetView(), Camera->GetProjection());
		PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());
	}

	void DeferredRenderer::Submit(Application* InApp)
	{
		// todo --- do this in parallel 
		PerFrameData.GeometryBucket.Sort();
		PerFrameData.FinalPassBucket.Sort();
		// SortTask.Join();
		PerFrameData.GeometryBucket.Submit(InApp);
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
		PerFrameData.GeometryBucket.Free();
	}
}