#include "Renderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include <random>

namespace Dawn
{
	void AllocateSSAOResources(GfxGDI* InGDI, SSAOData* InData)
	{
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
			InData->Kernel.push_back(sample);
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

		InData->NoiseTextureId = InGDI->CreateTexture(Desc, nullptr);
	}

	void DeferredRenderer::AllocateTransientData(Application* InApp)
	{
		auto GDI = InApp->GetGDI();
		auto Settings = InApp->GetSettings();

		GfxRenderBuffer* GBuffer;
		auto IdBuffer = GDI->CreateRenderBuffer(&GBuffer);
		if (IdBuffer.IsValid)
		{
			GBuffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
			GBuffer->AttachColorTarget(1, Settings->Width, Settings->Height);	// Normal
			GBuffer->AttachColorTarget(2, Settings->Width, Settings->Height);	// Albedo, AO
			GBuffer->AttachColorTarget(3, Settings->Width, Settings->Height);	// Metallic,Roughness
			GBuffer->AttachDepthStencilTarget(Settings->Width, Settings->Height);
			TransientData.GBufferId = IdBuffer;
		}

		GfxRenderBuffer* ScreenBuffer;
		auto ScreenBufferId = GDI->CreateRenderBuffer(&ScreenBuffer);
		if (ScreenBufferId.IsValid)
		{
			ScreenBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, GfxTextureFormat::RGBA16F, GfxTextureFormat::RGBA, GfxMemoryType::Float);	// Position
			TransientData.FinalBufferId = ScreenBufferId;
		}

		GfxRenderBuffer* SSAOBuffer;
		auto SSAOBufferId = GDI->CreateRenderBuffer(&SSAOBuffer);
		if (SSAOBufferId.IsValid)
		{
			SSAOBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, RGBA16F, R);	
			TransientData.SSAOBufferId = SSAOBufferId;
		}

		AllocateSSAOResources(GDI.get(), &SSAOData);


	/*	GfxRenderBuffer* SSAOBlurBuffer;
		IdBuffer = GDI->CreateRenderBuffer(&SSAOBlurBuffer);
		if (IdBuffer.IsValid)
		{
			SSAOBlurBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, RGBA16F, R);
			TransientData.SSAOBlurBufferId = IdBuffer;
		}*/
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

		/*if (auto SSAOBlurBuffer = InGDI->GetRenderBuffer(TransientData.SSAOBlurBufferId))
		{
			SSAOBlurBuffer->AttachColorTarget(0, InWidth, InHeight, RGBA16F, R);
		}*/
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, World* InWorld)
	{
		BROFILER_EVENT("Rendering_BeginFrame")
		// todo --- change the way how we obtain the camera ... for now we just use the default one!

		glDisable(GL_BLEND);

		auto Camera = InWorld->GetCamera(0);
		PerFrameData.GeometryBucket.Reset(2048, TransientData.GBufferId, Camera->GetView(), Camera->GetProjection());
		PerFrameData.SSAOBucket.Reset(2, TransientData.SSAOBufferId, mat4(), mat4());
		PerFrameData.LightingBucket.Reset(4, TransientData.FinalBufferId, mat4(), mat4());
		PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());

	}

	void DeferredRenderer::Submit(Application* InApp)
	{
		BROFILER_EVENT("Rendering_Submit")

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
		BROFILER_EVENT("Rendering_EndFrame")
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