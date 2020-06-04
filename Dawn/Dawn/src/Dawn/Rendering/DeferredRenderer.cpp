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
#include "Core/GDI/Base/GfxShaderResourceCache.h"
#include "Core/GDI/Base/GfxGDI.h"
#include "Core/GDI/inc_gfx.h"
#include "Core/Math.h"
#include "Core/GDI/Base/GfxRTBundle.h"
#include "Core/GDI/Base/GfxVertexArrayObject.h"
#include "EntitySystem/Lights/LightSystem.h"
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
		desc.Format = Dawn::GfxFormat::RGBA8UN;
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
	RenderResourceHelper::LoadCommonShaders(InApplication->GetResourceSystem().get());
	RenderResourceHelper::CreatePSODefaults(InApplication);
	RenderResourceHelper::CreateConstantBuffers(InApplication->GetGDI().get());
	RenderResourceHelper::CreateSamplers(InApplication->GetGDI().get());
	TransientData.QuadVAOId = GfxPrimitiveFactory::AllocateQuad(InApplication->GetGDI().get(), vec2(1.0, 1.0f), 1.0f)->GetId();

	// Push passes in the order you want them to be processed!!

	auto* shadowPass = BeginPass("Shadow", 2048);
	shadowPass->IsActive = false;
	shadowPass->Setup = [&](RenderPass* InPass)
	{
		auto Settings = InApplication->GetSettings();
		InPass->RenderTargets.SetGDI(InApplication->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB);
	};
	shadowPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		InPass->Bucket.Reset(mat4(), mat4());

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
	auto* geometryPass = BeginPass("Geometry", 2048);
	geometryPass->Setup = [](RenderPass* InPass) 
	{
		auto Settings = g_Application->GetSettings();
		InPass->RenderTargets.SetGDI(g_Application->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB); // Position
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB); // Normal
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB); // Albedo, AO
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB); // Metallic,Roughness
		InPass->RenderTargets.SetDepthStencilTarget(Settings->Width, Settings->Height);

		auto psoId = RenderResourceHelper::GetCachedPSO("GeometryPass");

		auto gdi = g_Application->GetGDI();
		auto* pso = gdi->GetPipelineState(psoId);
		pso->GetResourceCache(GfxShaderType::Vertex)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerAppData));
		pso->GetResourceCache(GfxShaderType::Vertex)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerFrameData));
		pso->GetResourceCache(GfxShaderType::Vertex)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerObjectData));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::MaterialData));

		auto psoInstanceId = RenderResourceHelper::GetCachedPSO("GeometryPassInstanced");
		auto* psoInstanced = gdi->GetPipelineState(psoInstanceId);
		psoInstanced->GetResourceCache(GfxShaderType::Vertex)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerAppData));
		psoInstanced->GetResourceCache(GfxShaderType::Vertex)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerFrameData));
		psoInstanced->GetResourceCache(GfxShaderType::Pixel)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::MaterialData));
	};

	geometryPass->PerFrameSetup = [](RenderPass* InPass) 
	{
		auto* cam = World::GetActiveCamera();
		auto gdi = g_Application->GetGDI();
		gdi->Clear();

		InPass->Bucket.Reset(cam->GetView(), cam->GetProjection());

		CBPerFrameData frameData = {};
		frameData.View = cam->GetView();
		frameData.CameraPosition = cam->GetTransform(cam->WorldRef)->GetWorldPosition();
		gdi->UpdateConstantBuffer(CommonConstantBuffers::PerFrameData, &frameData, sizeof(frameData));

		gdi->SetRenderTargetBundle(&InPass->RenderTargets);
		gdi->ClearWithColor(InPass->RenderTargets.GetRenderTargetId(0), vec4(0.0f, 0.0f, 0.0f, 0.0f));
		gdi->ClearWithColor(InPass->RenderTargets.GetRenderTargetId(1), vec4(0.0f, 0.0f, 0.0f, 0.0f));
		gdi->ClearWithColor(InPass->RenderTargets.GetRenderTargetId(2), vec4(0.0f, 0.0f, 0.0f, 0.0f));
		gdi->ClearWithColor(InPass->RenderTargets.GetRenderTargetId(3), vec4(0.0f, 0.0f, 0.0f, 0.0f));
		gdi->ClearDepthStencil(InPass->RenderTargets.GetDepthStencilTargetId(), 1.0f, 0);
	};
	PushPass(geometryPass);

	// -----------------------------------------------------------------------------------------
	auto* lightingPass = BeginPass("Lighting", 0);
	lightingPass->Setup = [&](RenderPass* InPass)
	{
		auto gdi = g_Application->GetGDI();
		auto Settings = g_Application->GetSettings();

		InPass->RenderTargets.SetGDI(g_Application->GetGDI());
		InPass->RenderTargets.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA8UN_SRGB); // Position

		auto psoId = RenderResourceHelper::GetCachedPSO("PBRLightingPass"); // todo(gb): cache the id?! :D
		auto pso = gdi->GetPipelineState(psoId);

		pso->GetResourceCache(GfxShaderType::Pixel)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::DirLightData));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PerFrameData));
		//pso->GetResourceCache(GfxShaderType::Pixel)->BindConstantBuffer(gdi->GetBuffer(CommonConstantBuffers::PointLightData));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindSampler(gdi->GetSampler(CommonSamplers::DefaultSampler));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindSampler(gdi->GetSampler(CommonSamplers::DefaultSampler));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindSampler(gdi->GetSampler(CommonSamplers::DefaultSampler));
		pso->GetResourceCache(GfxShaderType::Pixel)->BindSampler(gdi->GetSampler(CommonSamplers::DefaultSampler));
	};

	lightingPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		auto* cam = World::GetActiveCamera();
		auto gdi = g_Application->GetGDI();
		auto renderer = g_Application->GetRenderer();

		LightSystem::Update(g_Application->GetWorld().get());

		auto* lightingPass = renderer->GetPass(RenderPassId::Geometry);
		auto rt0 = lightingPass->RenderTargets.GetRenderTargetShaderView(0);
		auto rt1 = lightingPass->RenderTargets.GetRenderTargetShaderView(1);
		auto rt2 = lightingPass->RenderTargets.GetRenderTargetShaderView(2);
		auto rt3 = lightingPass->RenderTargets.GetRenderTargetShaderView(3);

		auto psoId = RenderResourceHelper::GetCachedPSO("PBRLightingPass"); // todo(gb): cache the id?! :D
		auto pso = gdi->GetPipelineState(psoId);

		auto psoCache = pso->GetResourceCache(GfxShaderType::Pixel);

		gdi->Clear();
		gdi->SetPipelineState(psoId);
		gdi->SetRenderTargetBundle(&InPass->RenderTargets);
		gdi->ClearWithColor(InPass->RenderTargets.GetRenderTargetId(0), vec4(0.0f, 0.0f, 0.0f, 0.0f));

		psoCache->ClearShaderResourceViews();
		psoCache->BindShaderResourceView(rt0);
		psoCache->BindShaderResourceView(rt1);
		psoCache->BindShaderResourceView(rt2);
		psoCache->BindShaderResourceView(rt3);
		gdi->CommitShaderResources(psoId);
		gdi->BindPipelineShaders();
		gdi->DrawIndexed(this->TransientData.QuadVAOId);
	};
	PushPass(lightingPass);

	// -----------------------------------------------------------------------------------------
	/*auto* ssaoPass = BeginPass("SSAO");
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
	PushPass(ssaoPass);*/

	// -----------------------------------------------------------------------------------------
	auto* fxaaPass = BeginPass("FXAA", 0);
	fxaaPass->Setup = [&](RenderPass* InPass)
	{
		auto gdi = g_Application->GetGDI();
		auto psoId = RenderResourceHelper::GetCachedPSO("FXAAPass"); // todo(gb): cache the id?! :D
		auto pso = gdi->GetPipelineState(psoId);
		pso->GetResourceCache(GfxShaderType::Pixel)->BindSampler(gdi->GetSampler(CommonSamplers::DefaultSampler));
	};

	fxaaPass->PerFrameSetup = [&](RenderPass* InPass)
	{
		auto* cam = World::GetActiveCamera();
		auto gdi = g_Application->GetGDI();
		auto renderer = g_Application->GetRenderer();
		auto* lightingPass = renderer->GetPass(RenderPassId::Lighting);
		auto rt0 = lightingPass->RenderTargets.GetRenderTargetShaderView(0);
		auto psoId = RenderResourceHelper::GetCachedPSO("FXAAPass"); // todo(gb): cache the id?! :D
		auto pso = gdi->GetPipelineState(psoId);

		auto psoCache = pso->GetResourceCache(GfxShaderType::Pixel);

		gdi->Clear();
		gdi->SetPipelineState(psoId);
		gdi->SetToBackbuffer();
		gdi->ClearWithColor(gdi->GetBackbufferId(), cam->ClearColor);

		psoCache->ClearShaderResourceViews();
		psoCache->BindShaderResourceView(rt0);
		gdi->BindPipelineShaders();
		gdi->CommitShaderResources(psoId);
		gdi->DrawIndexed(this->TransientData.QuadVAOId);
	};
	PushPass(fxaaPass);
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

	InGDI->SetViewport(0, 0, InCamera->Width, InCamera->Height);
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
		{
			pass->PerFrameSetup(pass);
			pass->Bucket.Submit(InApp);
		}
	}
}

void Dawn::DeferredRenderer::EndFrame(GfxGDI* InGDI)
{
	BROFILER_EVENT("Rendering_EndFrame");

	if (OnPostRender)
		OnPostRender(InGDI, this);

	InGDI->Present();

	for (auto* pass : Passes)
	{
		pass->Bucket.Free();
	}
}