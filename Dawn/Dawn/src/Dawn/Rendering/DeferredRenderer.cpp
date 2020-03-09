#include "stdafx.h"
#include "DeferredRenderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "UI/Editor/imgui_debug.h"
#include "Core/GDI/Base/GfxRTBundle.h"
#include "Core/GDI/Base/GfxPipelineStateObject.h"
#include "Core/GDI/Base/GfxInputLayouts.h"
#include "Core/GDI/Base/GfxGDI.h"
#include "Core/GDI/inc_gfx.h"
#include "Core/Math.h"
#include "Core/GDI/Base/GfxRTBundle.h"
#include <random>

namespace Dawn
{
	void AllocateSSAOResources(Dawn::GfxGDI* InGDI, Dawn::SSAOSettings* InData)
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

		Dawn::GfxTextureDesc desc = {};
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Usage = Dawn::GfxUsageFlags::Usage_Default;
		desc.CpuAccess = Dawn::GfxCpuAccessFlags::CpuAccess_None;
		desc.BindFlags = Dawn::GfxBindFlags::Bind_ShaderResource;
		desc.Width = 4;
		desc.Height = 4;
		desc.Format = Dawn::GfxFormat::RGB16F;
		desc.Wrap.WrapS = 0;
		desc.Wrap.WrapT = 0;
		desc.SampleQuality = 0;
		desc.SampleCount = 1;

		Dawn::GfxTextureData data = {};
		data.ChannelsPerPixel = 3;
		data.Data = &ssaoNoise[0];
		data.Size = ssaoNoise.size() * sizeof(float) * 3;

		InData->NoiseTextureId = InGDI->CreateTexture(desc, data, nullptr);
	}
}

Dawn::GfxResId gDefaultPSO;

void Dawn::DeferredRenderer::CreatePasses(Application* InApplication)
{
	// Push passes in the order you want them to be processed!!

	auto* shadowPass = BeginPass("Shadow");
	shadowPass->IsActive = false;
	shadowPass->Setup = [&](RenderPass* InPass)
	{
		auto Settings = InApplication->GetSettings();
		InPass->RenderTargets.SetGDI(InApplication->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F);
	};
	shadowPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		InPass->Bucket.Reset(2048, &InPass->RenderTargets, mat4(), mat4());

		Shared<World> world = InApplication->GetWorld();
		auto directionalLights = world->GetComponentsByType<DirectionalLight>();
		LightUtils::CalculateOrthoLightMatrix(world.get(), directionalLights[0], 0.1f, 1000.0f);
		
		auto ShadowPassCmd = InPass->Bucket.AddCommand<Draw::ShadowPassData>(1u);
		ShadowPassCmd->Width = ShadowSettings.Width;
		ShadowPassCmd->Height = ShadowSettings.Height;
		ShadowPassCmd->ShaderId = CommonShaderHandles::ShadowMapCompute;
		ShadowPassCmd->LightSpace = directionalLights[0]->LightSpace;
	};
	PushPass(shadowPass);

	// -----------------------------------------------------------------------------------------
	auto* geometryPass = BeginPass("Geometry");
	geometryPass->Setup = [](RenderPass* InPass) 
	{
		auto Settings = g_Application->GetSettings();
		InPass->RenderTargets.SetGDI(g_Application->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Position
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Normal
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Albedo, AO
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Metallic,Roughness
		InPass->RenderTargets.SetDepthStencilTarget(Settings->Width, Settings->Height, GfxFormat::D24S8);
	};
	geometryPass->PerFrameSetup = [](RenderPass* InPass) 
	{
		auto* cam = World::GetActiveCamera();
		auto gdi = g_Application->GetGDI();

		InPass->Bucket.Reset(2048, &InPass->RenderTargets, cam->GetView(), cam->GetProjection());

		gdi->ClearWithColor(gdi->GetBackBufferId(), vec4(0.4f, 0.5f, 0.7f, 1.0f));
	};
	PushPass(geometryPass);

	// -----------------------------------------------------------------------------------------
	/*auto* lightingPass = BeginPass("Lighting");
	lightingPass->Setup = [&](RenderPass* InPass)
	{
		
	};
	lightingPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		auto ClearColor = InPass->Bucket.AddCommand<Draw::ClearSceneWithColorData>(0u);
		ClearColor->ClearColor = World::GetActiveCamera()->ClearColor;

		auto LightingPassData = InPass->Bucket.AppendCommand<Draw::LightingPassData>(ClearColor);
		LightingPassData->ShaderId = CommonShaderHandles::LightingPass;
		/*LightingPassData->GBufferId = InPass->Bucket.GBufferId;
		LightingPassData->FSQuadVAOId = FinalPassQuadId;
		LightingPassData->ViewPosition = World::GetActiveCamera()->GetTransform()->Position;
		LightingPassData->SSAOBufferId = SSAOBlurBufferId;*/
	/*};
	PushPass(lightingPass);

	// -----------------------------------------------------------------------------------------
	auto* ssaoPass = BeginPass("SSAO");
	ssaoPass->Setup = [&](RenderPass* InPass)
	{
		auto Settings = InApplication->GetSettings();
		InPass->RenderTargets.SetGDI(InApplication->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::R16F);	// SSAO (R)

		AllocateSSAOResources(InApplication->GetGDI().get(), &SSAOSettings);
	};
	ssaoPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		InPass->Bucket.Reset(2, &InPass->RenderTargets, mat4(), mat4());

		auto ClearSSAO = InPass->Bucket.AddCommand<Draw::ClearSceneWithColorData>(0u);
		ClearSSAO->ClearColor = vec4(1, 1, 1, 1);

		auto SSAOComputePass = InPass->Bucket.AppendCommand<Draw::SSAOComputePassData>(ClearSSAO);
		//SSAOComputePass->GBufferId = GetPass(RenderPassId::Geometry)->RenderTargets;
		//SSAOComputePass->FSQuadVAOId = FinalPassQuadId;
		SSAOComputePass->ShaderId = CommonShaderHandles::SSAOCompute;
		SSAOComputePass->SSAOKernelData = &SSAOSettings.Kernel;
		SSAOComputePass->NoiseTextureId = SSAOSettings.NoiseTextureId;
		SSAOComputePass->Radius = SSAOSettings.Radius;
		SSAOComputePass->Bias = SSAOSettings.Bias;
		SSAOComputePass->Power = SSAOSettings.Power;

		auto SSAOBlurPass = InPass->Bucket.AppendCommand<Draw::SSAOBlurPassData>(SSAOComputePass);
		SSAOBlurPass->ShaderId = CommonShaderHandles::SSAOBlur;
		//SSAOBlurPass->SSAOBlurBufferId = Renderer->TransientData.SSAOBlurBufferId;
		//SSAOBlurPass->SSAOBufferId = Renderer->TransientData.SSAOBufferId;
		//SSAOBlurPass->FSQuadVAOId = FinalPassQuadId;
	};
	PushPass(ssaoPass);

	// -----------------------------------------------------------------------------------------
	auto* fxaaPass = BeginPass("FXAA");
	fxaaPass->Setup = [&](RenderPass* InPass)
	{
		auto Settings = InApplication->GetSettings();
		InPass->RenderTargets.SetGDI(InApplication->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F);
	};
	fxaaPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		InPass->Bucket.Reset(2, &InPass->RenderTargets, mat4(), mat4());

		auto ClearFinalPass = InPass->Bucket.AddCommand<Draw::ClearSceneWithColorData>(0u);
		ClearFinalPass->ClearColor = World::GetActiveCamera()->ClearColor;

		auto FXAAPass = InPass->Bucket.AppendCommand<Draw::FXAAData>(ClearFinalPass);
		FXAAPass->ShaderId = CommonShaderHandles::FXAA;
		//FXAAPass->RenderBufferId = Renderer->TransientData.FinalBufferId;
		//FXAAPass->FSQuadVAOId = FinalPassQuadId;
	};
	PushPass(fxaaPass);*/
}
void Dawn::DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
{
	InGDI->Resize(InWidth, InHeight);
}

void Dawn::DeferredRenderer::BeginFrame(GfxGDI* InGDI, Camera* InCamera)
{
	BROFILER_EVENT("Rendering_BeginFrame");
	D_ASSERT(InCamera != nullptr, "There is no active camera to use for rendering!");

	Stats.DrawCalls = 0;
	PerFrameData.Camera = InCamera;

	for(auto* pass : Passes)
	{
		D_ASSERT(pass->PerFrameSetup != nullptr, "");
		if(pass->IsActive)
			pass->PerFrameSetup(pass);
	}
}

void Dawn::DeferredRenderer::Submit(Application* InApp)
{
	BROFILER_EVENT("Rendering_Submit");

	// todo (gb): do this in parallel.
	for (auto* pass : Passes)
	{
		if (pass->IsActive)
			pass->Bucket.Sort();
	}

	for (auto* pass : Passes)
	{
		if (pass->IsActive)
			pass->Bucket.Submit(InApp);
	}
}

void Dawn::DeferredRenderer::EndFrame(GfxGDI* InGDI)
{
	BROFILER_EVENT("Rendering_EndFrame")

	if (OnPostRender)
		OnPostRender(InGDI, this);

	InGDI->Present();

	for (auto* pass : Passes)
	{
		if (pass->IsActive)
			pass->Bucket.Free();
	}
}