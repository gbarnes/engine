#include "Renderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include <random>

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
			TransientData.GBufferId = IdBuffer;
		}

		GfxRenderBuffer* ScreenBuffer;
		IdBuffer = GDI->CreateRenderBuffer(&ScreenBuffer);
		if (IdBuffer.IsValid)
		{
			ScreenBuffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
			TransientData.FinalBufferId = IdBuffer;
		}

		GfxRenderBuffer* SSAOBuffer;
		IdBuffer = GDI->CreateRenderBuffer(&SSAOBuffer);
		if (IdBuffer.IsValid)
		{
			SSAOBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, RGBA16F, R);	
			TransientData.SSAOBufferId = IdBuffer;
		}

		std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
		std::default_random_engine generator;
		float scale = 0;
		for (unsigned int i = 0; i < 64; ++i)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			scale = Math::Lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			SSAOData.Kernel.push_back(sample);
		}

		std::vector<vec3> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++)
		{
			vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);
			ssaoNoise.push_back(noise);
		}

		GfxTextureDesc Desc
		{
			GfxTextureFormat::RGB16F,
			GfxMemoryType::Float,
			GfxTextureFormat::RGB,
			&ssaoNoise[0],
			4,
			4,
			0,
			{ GL_REPEAT, GL_REPEAT }, // wrap settings 
			{ GL_NEAREST, GL_NEAREST }, // filter settings
			false
		};

		SSAOData.NoiseTextureId = GDI->CreateTexture(Desc, nullptr);
	}

	void DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
	{
		if (auto Buffer = InGDI->GetRenderBuffer(TransientData.GBufferId))
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

		if (auto SSAOBuffer = InGDI->GetRenderBuffer(TransientData.SSAOBufferId))
		{
			SSAOBuffer->AttachColorTarget(0, InWidth, InHeight, RGBA16F, R);
		}
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, World* InWorld)
	{
		glDisable(GL_BLEND);

		// todo --- change the way how we obtain the camera ... for now we just use the default one!
		auto Camera = InWorld->GetCamera(0);
		PerFrameData.GeometryBucket.Reset(2048, TransientData.GBufferId, Camera->GetView(), Camera->GetProjection());
		PerFrameData.SSAOBucket.Reset(2, TransientData.SSAOBufferId, mat4(), mat4());
		PerFrameData.LightingBucket.Reset(4, TransientData.FinalBufferId, mat4(), mat4());
		PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());
	}

	void DeferredRenderer::Submit(Application* InApp)
	{
		// todo --- do this in parallel 
		PerFrameData.GeometryBucket.Sort();
		PerFrameData.SSAOBucket.Sort();
		PerFrameData.LightingBucket.Sort();
		PerFrameData.FinalPassBucket.Sort();
		// SortTask.Join();
		PerFrameData.GeometryBucket.Submit(InApp);
		PerFrameData.SSAOBucket.Submit(InApp);
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
		PerFrameData.SSAOBucket.Free();
		PerFrameData.GeometryBucket.Free();
	}
}