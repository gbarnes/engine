#include "stdafx.h"
#include "Renderer.h"
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Camera/Camera.h"
#include "Vendor/ImGui/ImGuiWrapper.h"
#include "UI/Editor/imgui_editor_functions.h"
#include "UI/Editor/imgui_debug.h"
#include "Core/GDI/Base/GfxRTBundle.h"
#include "Core/GDI/Base/GfxPipelineStateObject.h"
#include "Core/GDI/Base/GfxInputLayouts.h"
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

		GfxTextureDesc desc = {};
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Usage = GfxUsageFlags::Usage_Default;
		desc.CpuAccess = GfxCpuAccessFlags::CpuAccess_None;
		desc.BindFlags = GfxBindFlags::Bind_ShaderResource;
		desc.Width = 4;
		desc.Height = 4;
		desc.Format = GfxFormat::RGB16F;
		desc.Wrap.WrapS = 0;
		desc.Wrap.WrapT = 0;
		desc.SampleQuality = 0;
		desc.SampleCount = 1;

		GfxTextureData data = {};
		data.ChannelsPerPixel = 3;
		data.Data = &ssaoNoise[0];
		data.Size = ssaoNoise.size() * sizeof(float) * 3;
		
		InData->NoiseTextureId = InGDI->CreateTexture(desc, data, nullptr);
	}

	GfxResId gDefaultPSO;

	void DeferredRenderer::AllocateTransientData(Application* InApp)
	{
		auto GDI = InApp->GetGDI();
		auto Settings = InApp->GetSettings();
		
		AllocateSSAOResources(GDI.get(), &SSAOSettings);

		TransientData.GBuffer.SetGDI(GDI);
		TransientData.GBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Position
		TransientData.GBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Normal
		TransientData.GBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Albedo, AO
		TransientData.GBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F); // Metallic,Roughness
		TransientData.GBuffer.SetDephtStencilTarget(Settings->Width, Settings->Height, GfxFormat::D24S8);
		
		/*TransientData.FinalBuffer.SetGDI(GDI);
		TransientData.FinalBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F);	// Position

		TransientData.SSAOBlurBuffer.SetGDI(GDI);
		TransientData.SSAOBlurBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::R16F);	// SSAO (R)

		TransientData.SSAOBuffer.SetGDI(GDI);
		TransientData.SSAOBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::R16F);	// SSAO (R)

		TransientData.ShadowMapBuffer.SetGDI(GDI);
		TransientData.ShadowMapBuffer.AttachColorTarget(Settings->Width, Settings->Height, GfxFormat::RGBA32F);	
		*/

		GfxPipelineStateObject* pso = nullptr;
		GfxPipelineStateObjectDesc desc = {};
		desc.RasterizerState.CullMode = GfxCullMode::CullNone;
		desc.RasterizerState.FillMode = GfxFillMode::FillSolid;
		desc.RasterizerState.FrontCounterClockwise = 0;
		
		desc.BlendState.RenderTarget[0].BlendEnable = FALSE;
		desc.BlendState.RenderTarget[0].RenderTargetWriteMask = ( ( ( 1 | 2 )  | 4 )  | 8 ) ;

		desc.InputLayout = gPositionUVLayout;
		desc.TopologyType = GfxTopologyType::TopologyPolygon;
		desc.PixelShaderId = g_Application->GetResourceSystem()->FindShader(CommonShaderHandles::DebugPS)->GfxShaderId;
		desc.VertexShaderId = g_Application->GetResourceSystem()->FindShader(CommonShaderHandles::DebugVS)->GfxShaderId;
		gDefaultPSO = GDI->CreatePSO(desc, &pso);

		GDI->SetPipelineState(gDefaultPSO);
		

		auto* cache = pso->GetResourceCache(GfxShaderType::Vertex);

		// create per app data constant buffer
		{
			GfxConstantPerAppData appData;
			appData.Projection = mat4();

			GfxBuffer* constantBuffer;
			GfxBufferDesc desc;
			desc.AccessFlags = GfxCpuAccessFlags::CpuAccess_Write;
			desc.Usage = GfxUsageFlags::Usage_Dynamic;
			desc.BindFlags = GfxBindFlags::Bind_ConstantBuffer;
			desc.ByteWidth = sizeof(GfxConstantPerAppData);

			GfxBufferData data;
			data.Data = &appData;
			data.Size = sizeof(appData);

			TransientData.PerAppBuffer = GDI->CreateBuffer(desc, data, &constantBuffer);
			cache->BindConstantBuffer(constantBuffer);
		}

		// create per app data constant buffer
		{
			GfxConstantPerFrameData frameData; 
			frameData.View = mat4();

			GfxBuffer* constantBuffer;
			
			GfxBufferDesc desc;
			desc.AccessFlags = GfxCpuAccessFlags::CpuAccess_Write;
			desc.Usage = GfxUsageFlags::Usage_Dynamic;
			desc.BindFlags = GfxBindFlags::Bind_ConstantBuffer;
			desc.ByteWidth = sizeof(GfxConstantPerAppData);

			GfxBufferData data;
			data.Data = &frameData;
			data.Size = sizeof(frameData);

			TransientData.PerAppBuffer = GDI->CreateBuffer(desc, data, &constantBuffer);
			cache->BindConstantBuffer(constantBuffer);
		}

		// create per app data constant buffer
		{
			GfxConstantPerObjectData objectData;
			objectData.World = mat4();

			GfxBuffer* constantBuffer;

			GfxBufferDesc desc;
			desc.AccessFlags = GfxCpuAccessFlags::CpuAccess_Write;
			desc.Usage = GfxUsageFlags::Usage_Dynamic;
			desc.BindFlags = GfxBindFlags::Bind_ConstantBuffer;
			desc.ByteWidth = sizeof(GfxConstantPerAppData);

			GfxBufferData data;
			data.Data = &objectData;
			data.Size = sizeof(objectData);

			TransientData.PerObjectBuffer = GDI->CreateBuffer(desc, data, &constantBuffer);
			cache->BindConstantBuffer(constantBuffer);
		}
	}

	void DeferredRenderer::Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight)
	{
		/*if (auto GBuffer = InGDI->GetRenderBuffer(TransientData.GBufferId))
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

		if (auto ShadowMapBuffer = InGDI->GetRenderBuffer(TransientData.ShadowMapBufferId))
		{
			ShadowMapBuffer->AttachDepthTarget(InWidth, InHeight);
		}*/
	}

	void DeferredRenderer::BeginFrame(GfxGDI* InGDI, Camera* InCamera)
	{
		BROFILER_EVENT("Rendering_BeginFrame");
		D_ASSERT(InCamera != nullptr, "There is no active camera to use for rendering!");
		
		Stats.DrawCalls = 0;
		PerFrameData.Camera = InCamera;
		//PerFrameData.ShadowBucket.Reset(2048, TransientData.ShadowMapBuffer, mat4(), mat4());
		//PerFrameData.GeometryBucket.Reset(2048, TransientData.GBuffer, InCamera->GetView(), InCamera->GetProjection());
		//PerFrameData.SSAOBucket.Reset(2, TransientData.SSAOBufferId, mat4(), mat4());
		//PerFrameData.LightingBucket.Reset(4, TransientData.FinalBufferId, mat4(), mat4());
		//PerFrameData.FinalPassBucket.Reset(4, INVALID_HANDLE, mat4(), mat4());

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