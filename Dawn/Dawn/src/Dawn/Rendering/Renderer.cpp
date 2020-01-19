#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "UI/Editor/imgui_debug.h"
#include <random>
#include <glad.h>

namespace Dawn
{
	void AllocateSSAOResources(GfxGDI* InGDI, SSAOSettings* InData)
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
		TransientData.GBufferId = GDI->CreateRenderBuffer(&GBuffer);
		if (TransientData.GBufferId.IsValid)
		{
			GBuffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
			GBuffer->AttachColorTarget(1, Settings->Width, Settings->Height);	// Normal
			GBuffer->AttachColorTarget(2, Settings->Width, Settings->Height);	// Albedo, AO
			GBuffer->AttachColorTarget(3, Settings->Width, Settings->Height);	// Metallic,Roughness
			GBuffer->AttachDepthStencilTarget(Settings->Width, Settings->Height);
		}

		GfxRenderBuffer* ScreenBuffer;
		TransientData.FinalBufferId = GDI->CreateRenderBuffer(&ScreenBuffer);
		if (TransientData.FinalBufferId.IsValid)
		{
			ScreenBuffer->AttachColorTarget(0, Settings->Width, Settings->Height);	// Position
		}

		GfxRenderBuffer* SSAOBuffer;
		TransientData.SSAOBufferId = GDI->CreateRenderBuffer(&SSAOBuffer);
		if (TransientData.SSAOBufferId.IsValid)
		{
			SSAOBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, RGBA16F, R);
		}

		AllocateSSAOResources(GDI.get(), &SSAOSettings);


		GfxRenderBuffer* SSAOBlurBuffer;
		TransientData.SSAOBlurBufferId = GDI->CreateRenderBuffer(&SSAOBlurBuffer);
		if (TransientData.SSAOBlurBufferId.IsValid)
		{
			SSAOBlurBuffer->AttachColorTarget(0, Settings->Width, Settings->Height, RGBA16F, R);
		}

		GfxRenderBuffer* ShadowMapBuffer;
		TransientData.ShadowMapBufferId = GDI->CreateRenderBuffer(&ShadowMapBuffer);
		if (TransientData.ShadowMapBufferId.IsValid)
		{
			ShadowMapBuffer->AttachDepthTarget(Settings->Width, Settings->Height);
		}
	}

	void DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
	{
		if (auto GBuffer = InGDI->GetRenderBuffer(TransientData.GBufferId))
		{
			GBuffer->AttachColorTarget(0, InWidth, InHeight);	// Position
			GBuffer->AttachColorTarget(1, InWidth, InHeight);	// Normal
			GBuffer->AttachColorTarget(2, InWidth, InHeight);	// Albedo
			GBuffer->AttachColorTarget(3, InWidth, InHeight);	// Albedo
			GBuffer->AttachDepthStencilTarget(InWidth, InHeight);
		}

		if (auto FinalBuffer = InGDI->GetRenderBuffer(TransientData.FinalBufferId))
		{
			FinalBuffer->AttachColorTarget(0, InWidth, InHeight);	// Position
			FinalBuffer->AttachDepthStencilTarget(InWidth, InHeight);
		}

		if (auto SSAOBuffer = InGDI->GetRenderBuffer(TransientData.SSAOBufferId))
		{
			SSAOBuffer->AttachColorTarget(0, InWidth, InHeight, RGBA16F, R);
		}

		if (auto SSAOBlurBuffer = InGDI->GetRenderBuffer(TransientData.SSAOBlurBufferId))
		{
			SSAOBlurBuffer->AttachColorTarget(0, InWidth, InHeight, RGBA16F, R);
		}
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, Camera* InCamera)
	{
		BROFILER_EVENT("Rendering_BeginFrame");
		D_ASSERT(InCamera != nullptr, "There is no active camera to use for rendering!");
		
		PerFrameData.Camera = InCamera;
		PerFrameData.ShadowBucket.Reset(2048, TransientData.ShadowMapBufferId, mat4(), mat4());
		PerFrameData.GeometryBucket.Reset(2048, TransientData.GBufferId, InCamera->GetView(), InCamera->GetProjection());
		PerFrameData.SSAOBucket.Reset(2, TransientData.SSAOBufferId, mat4(), mat4());
		PerFrameData.LightingBucket.Reset(4, TransientData.FinalBufferId, mat4(), mat4());
		PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());

	}

	void DeferredRenderer::Submit(Application* InApp)
	{
		BROFILER_EVENT("Rendering_Submit")

		// todo --- do this in parallel 
		PerFrameData.ShadowBucket.Sort();
		PerFrameData.GeometryBucket.Sort();
		// SortTask.Join();

		PerFrameData.ShadowBucket.Submit(InApp);
		PerFrameData.GeometryBucket.Submit(InApp);
		PerFrameData.SSAOBucket.Submit(InApp);
		PerFrameData.LightingBucket.Submit(InApp);
		PerFrameData.FinalPassBucket.Submit(InApp);
	}

	void DeferredRenderer::EndFrame(GfxGDI* InGDI)
	{
		BROFILER_EVENT("Rendering_EndFrame")

		if(OnPostRender)
			OnPostRender(InGDI, this);

		InGDI->Present();

		PerFrameData.FinalPassBucket.Free();
		PerFrameData.LightingBucket.Free();
		PerFrameData.SSAOBucket.Free();
		PerFrameData.GeometryBucket.Free();
		PerFrameData.ShadowBucket.Free();
	}
}