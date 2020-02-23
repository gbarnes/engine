#include "stdafx.h"
#include "DrawCallCommands.h"
#include "glad.h"
#include <Brofiler.h>
#include "Application.h"
#include "EntitySystem/World.h"
#include "EntitySystem/Lights/LightComponents.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/Lights/LightSystem.h"
#include "DeferredRenderer.h"
#include "EntitySystem/Camera/Camera.h"

namespace Dawn
{
	const DrawDispatchFunction Draw::DrawIndexedData::DRAW_FUNCTION = &Draw::DrawIndexed;
	const DrawDispatchFunction Draw::DrawInstancedData::DRAW_FUNCTION = &Draw::DrawInstanced;
	const DrawDispatchFunction Draw::DrawVertexData::DRAW_FUNCTION = &Draw::DrawVertex;
	const DrawDispatchFunction Draw::ClearSceneData::DRAW_FUNCTION = &Draw::Clear;
	const DrawDispatchFunction Draw::ClearSceneWithColorData::DRAW_FUNCTION = &Draw::ClearWithColor;
	const DrawDispatchFunction Draw::SetViewportData::DRAW_FUNCTION = &Draw::SetViewport;
	const DrawDispatchFunction Draw::FXAAData::DRAW_FUNCTION = &Draw::FXAAPass;
	const DrawDispatchFunction Draw::LightingPassData::DRAW_FUNCTION = &Draw::LightingPass;
	const DrawDispatchFunction Draw::SSAOComputePassData::DRAW_FUNCTION = &Draw::SSAOComputePass;
	const DrawDispatchFunction Draw::SSAOBlurPassData::DRAW_FUNCTION = &Draw::SSAOBlurPass;
	const DrawDispatchFunction Draw::ShadowPassData::DRAW_FUNCTION = &Draw::ShadowPass;
	const DrawDispatchFunction Draw::SetStateData::DRAW_FUNCTION = &Draw::SetState;

	DAWN_API void Draw::DrawIndexed(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const DrawIndexedData* RenderData = static_cast<const DrawIndexedData*>(data);

		// todo (gb): change this?!
		//InRenderer->CurrentShader->SetInt("isInstanced", 0);
		//InRenderer->CurrentShader->SetMat4("model", RenderData->Model);
		InGDI->DrawIndexed(RenderData->VertexArrayId);

		InRenderer->Stats.DrawCalls += 1;
	}


	DAWN_API void Draw::DrawInstanced(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const DrawInstancedData* RenderData = static_cast<const DrawInstancedData*>(data);
		//InRenderer->CurrentShader->SetInt("isInstanced", 1);
		InGDI->DrawInstanced(RenderData->VertexArrayId, RenderData->Amount);

		InRenderer->Stats.DrawCalls += 1;
	}

	DAWN_API void Draw::DrawVertex(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const DrawVertexData* RenderData = static_cast<const DrawVertexData*>(data);
		//InRenderer->CurrentShader->SetInt("isInstanced", 0);
		//InRenderer->CurrentShader->SetMat4("model", RenderData->Model);
		InGDI->DrawArray(RenderData->VertexArrayId);

		InRenderer->Stats.DrawCalls += 1;
	}

	DAWN_API void Draw::Clear(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const ClearSceneData* RenderData = static_cast<const ClearSceneData*>(data);
		InGDI->Clear();
	}

	DAWN_API void Draw::ClearWithColor(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const ClearSceneWithColorData* RenderData = static_cast<const ClearSceneWithColorData*>(data);
		InGDI->ClearWithColor(RenderData->ClearColor);
	}

	DAWN_API void Draw::SetViewport(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		const SetViewportData* RenderData = static_cast<const SetViewportData*>(data);
		InGDI->SetViewport(0, 0, RenderData->Width, RenderData->Height);
	}

	DAWN_API void Draw::LightingPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("Lighting Pass")

		
		const LightingPassData* RenderData = static_cast<const LightingPassData*>(data);
		auto LightingPassShader = InGDI->GetShader(RenderData->ShaderId);
		/*auto GBuffer = InGDI->GetRenderBuffer(RenderData->GBufferId);
		auto SSAOBuffer = InGDI->GetRenderBuffer(RenderData->SSAOBufferId);
		auto ShadowBuffer = InGDI->GetRenderBuffer(InRenderer->TransientData.ShadowMapBufferId);

		ScopedGfxBind<GfxShader> Bind(LightingPassShader);
		{
			InGDI->ActivateTextureSlot(0);
			GBuffer->BindColorTarget(0);
			
			InGDI->ActivateTextureSlot(1);
			GBuffer->BindColorTarget(1);
			
			InGDI->ActivateTextureSlot(2);
			GBuffer->BindColorTarget(2);

			InGDI->ActivateTextureSlot(3);
			GBuffer->BindColorTarget(3);

			InGDI->ActivateTextureSlot(4);
			SSAOBuffer->BindColorTarget(0);

			InGDI->ActivateTextureSlot(4);
			SSAOBuffer->BindColorTarget(0);

			InGDI->ActivateTextureSlot(5);
			ShadowBuffer->BindDepthBuffer();


			LightingPassShader->SetInt("gPosition", 0);
			LightingPassShader->SetInt("gNormal", 1);
			LightingPassShader->SetInt("gAlbedoAO", 2);
			LightingPassShader->SetInt("gMetallicRoughness", 3);
			LightingPassShader->SetInt("gSSAO", 4);
			LightingPassShader->SetInt("gShadowMap", 5);

			LightingPassShader->SetMat4("view", InRenderer->PerFrameData.Camera->GetView());
			LightingPassShader->SetVec3("viewPos", RenderData->ViewPosition);

			auto World = g_Application->GetWorld();
			auto PointLightTransformSet = World->GetComponentSets<PointLight, Transform>();

			// todo --- move this out of the draw call thus lights can be checked if active and so on!
			LightSystem::Update(World.get(), LightingPassShader);
			
			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			InGDI->ActivateTextureSlot(5);
			ShadowBuffer->UnbindDepthBuffer();

			InGDI->ActivateTextureSlot(4);
			SSAOBuffer->UnbindColorTarget(0);

			InGDI->ActivateTextureSlot(3);
			GBuffer->UnbindColorTarget(3);

			InGDI->ActivateTextureSlot(2);
			GBuffer->UnbindColorTarget(2);

			InGDI->ActivateTextureSlot(1);
			GBuffer->UnbindColorTarget(1);

			InGDI->ActivateTextureSlot(0);
			GBuffer->UnbindColorTarget(0);
		}*/
	}

	DAWN_API void Draw::FXAAPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("FXAA  Pass")
		// todo --- this might be better of using actual indexed draw call and a more
		//			generic approach since we for example have a command to clear.
		const FXAAData* RenderData = static_cast<const FXAAData*>(data);
		/*auto FinalPassShader = InGDI->GetShader(RenderData->ShaderId);
		auto RenderBuffer = InGDI->GetRenderBuffer(RenderData->RenderBufferId);

		ScopedGfxBind<GfxShader> Bind(FinalPassShader);
		{
			InGDI->ActivateTextureSlot(0);
			RenderBuffer->BindColorTarget(0);

			FinalPassShader->SetInt("screenTexture", 0);

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			RenderBuffer->UnbindColorTarget(0);
		}*/
	}

	DAWN_API void Draw::SSAOBlurPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("SSAO Blur Pass");
		const SSAOBlurPassData* RenderData = static_cast<const SSAOBlurPassData*>(data);
		/*auto Shader = InGDI->GetShader(RenderData->ShaderId);
		auto SSAOBuffer = InGDI->GetRenderBuffer(RenderData->SSAOBufferId);
		auto SSAOBlurBuffer = InGDI->GetRenderBuffer(RenderData->SSAOBlurBufferId);

		SSAOBlurBuffer->Bind();

		ScopedGfxBind<GfxShader> Bind(Shader);
		{
			InGDI->ActivateTextureSlot(0);
			SSAOBuffer->BindColorTarget(0);

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			InGDI->ActivateTextureSlot(0);
			SSAOBuffer->UnbindColorTarget(0);
		}*/
	}

	DAWN_API void Draw::ShadowPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("Shadowmap Pass");
		const ShadowPassData* RenderData = static_cast<const ShadowPassData*>(data);
		InRenderer->CurrentShader = InGDI->GetShader(RenderData->ShaderId);
		//glCullFace(GL_FRONT);
		InGDI->SetViewport(0, 0, InRenderer->PerFrameData.Camera->Width, InRenderer->PerFrameData.Camera->Height);
		InGDI->ClearWithColor(vec4(1, 1, 1, 1));
		//InRenderer->CurrentShader->Bind();
		//InRenderer->CurrentShader->SetMat4("lightSpace", RenderData->LightSpace);
	}

	DAWN_API void Draw::SSAOComputePass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("SSAO Compute Pass");

		const SSAOComputePassData* RenderData = static_cast<const SSAOComputePassData*>(data);
		auto ComputeShader = InGDI->GetShader(RenderData->ShaderId);
	/*	auto GBuffer = InGDI->GetRenderBuffer(RenderData->GBufferId);
		auto NoiseTexture = InGDI->GetTexture(RenderData->NoiseTextureId);

		if (RenderData->Radius <= 0.0f)
			return;

		ScopedGfxBind<GfxShader> Bind(ComputeShader);
		{
			InGDI->ActivateTextureSlot(0);
			GBuffer->BindColorTarget(0);

			InGDI->ActivateTextureSlot(1);
			GBuffer->BindColorTarget(1);

			InGDI->ActivateTextureSlot(2);
			GBuffer->BindColorTarget(2);

			InGDI->ActivateTextureSlot(3);
			NoiseTexture->Bind();

		//	InGDI->ActivateTextureSlot(4);
		//	GBuffer->BindDepthBuffer();


			ComputeShader->SetInt("gPosition", 0);
			ComputeShader->SetInt("gNormal", 1);
			ComputeShader->SetInt("gAlbedo", 2);
			ComputeShader->SetInt("Noise", 3);
			ComputeShader->SetInt("gDepth", 4);

			ComputeShader->SetFloat("radius", RenderData->Radius);
			ComputeShader->SetFloat("bias", RenderData->Bias);
			ComputeShader->SetFloat("power", RenderData->Power);

			for (unsigned int i = 0; i < 64; ++i) {
				ComputeShader->SetVec3("samples[" + std::to_string(i) + "]", (*RenderData->SSAOKernelData)[i]);
			}
			
			ComputeShader->SetMat4("projection", InRenderer->PerFrameData.Camera->GetProjection());
			ComputeShader->SetMat4("view", InRenderer->PerFrameData.Camera->GetView());

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			//InGDI->ActivateTextureSlot(4);
		//	GBuffer->UnbindDepthBuffer();

			InGDI->ActivateTextureSlot(3);
			NoiseTexture->Unbind();

			InGDI->ActivateTextureSlot(2);
			GBuffer->UnbindColorTarget(2);

			InGDI->ActivateTextureSlot(1);
			GBuffer->UnbindColorTarget(1);

			InGDI->ActivateTextureSlot(0);
			GBuffer->UnbindColorTarget(0);
		}*/
	}


	DAWN_API void Draw::SetState(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data)
	{
		BROFILER_EVENT("Draw - SetState")
		//const SetStateData* RenderData = static_cast<const SetStateData*>(data);
		//InGDI->SetState(RenderData->State);
	}
}