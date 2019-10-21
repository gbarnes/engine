#include "DrawCallCommands.h"
#include "glad.h"

namespace Dawn
{
	const DrawDispatchFunction Draw::DrawIndexedData::DRAW_FUNCTION = &Draw::DrawIndexed;
	const DrawDispatchFunction Draw::DrawInstancedData::DRAW_FUNCTION = &Draw::DrawInstanced;
	const DrawDispatchFunction Draw::DrawVertexData::DRAW_FUNCTION = &Draw::DrawVertex;
	const DrawDispatchFunction Draw::ClearSceneData::DRAW_FUNCTION = &Draw::Clear;
	const DrawDispatchFunction Draw::ClearSceneWithColorData::DRAW_FUNCTION = &Draw::ClearWithColor;
	const DrawDispatchFunction Draw::SetViewportData::DRAW_FUNCTION = &Draw::SetViewport;
	const DrawDispatchFunction Draw::FinalPassCombineData::DRAW_FUNCTION = &Draw::CombineFinalPass;
	const DrawDispatchFunction Draw::LightingPassData::DRAW_FUNCTION = &Draw::LightingPass;
	const DrawDispatchFunction Draw::SSAOComputePassData::DRAW_FUNCTION = &Draw::SSAOComputePass;

	DAWN_API void Draw::DrawIndexed(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const DrawIndexedData* RenderData = static_cast<const DrawIndexedData*>(data);
		InShader->SetMat4("model", RenderData->Model);
		InGDI->DrawIndexed(RenderData->VertexArrayId);
	}

	DAWN_API void Draw::DrawInstanced(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const DrawInstancedData* RenderData = static_cast<const DrawInstancedData*>(data);
		InGDI->DrawInstanced(RenderData->VertexArrayId, RenderData->Amount);
	}

	DAWN_API void Draw::DrawVertex(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const DrawVertexData* RenderData = static_cast<const DrawVertexData*>(data);
		InShader->SetMat4("model", RenderData->Model);
		InGDI->DrawArray(RenderData->VertexArrayId);
	}

	DAWN_API void Draw::Clear(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const ClearSceneData* RenderData = static_cast<const ClearSceneData*>(data);
		InGDI->Clear();
	}

	DAWN_API void Draw::ClearWithColor(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const ClearSceneWithColorData* RenderData = static_cast<const ClearSceneWithColorData*>(data);
		InGDI->ClearWithColor(RenderData->ClearColor);
	}

	DAWN_API void Draw::SetViewport(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const SetViewportData* RenderData = static_cast<const SetViewportData*>(data);
		InGDI->SetViewport(0, 0, RenderData->Width, RenderData->Height);
	}

	DAWN_API void Draw::LightingPass(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const LightingPassData* RenderData = static_cast<const LightingPassData*>(data);
		auto LightingPassShader = InGDI->GetShader(RenderData->ShaderId);
		auto GBuffer = InGDI->GetRenderBuffer(RenderData->GBufferId);
		auto SSAOBuffer = InGDI->GetRenderBuffer(RenderData->SSAOBufferId);

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


			LightingPassShader->SetInt("gPosition", 0);
			LightingPassShader->SetInt("gNormal", 1);
			LightingPassShader->SetInt("gAlbedoAO", 2);
			LightingPassShader->SetInt("gMetallicRoughness", 3);
			LightingPassShader->SetInt("gSSAO", 4);

			LightingPassShader->SetMat4("view", RenderData->View);
			LightingPassShader->SetVec3("viewPos", RenderData->ViewPosition);
			LightingPassShader->SetVec3("lights[0].position", RenderData->LightPos);
			LightingPassShader->SetVec3("lights[0].color", RenderData->LightColor);
			LightingPassShader->SetFloat("lights[0].intensity", RenderData->LightIntensity);

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

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
		}
	}

	DAWN_API void Draw::CombineFinalPass(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		// todo --- this might be better of using actual indexed draw call and a more
		//			generic approach since we for example have a command to clear.
		const FinalPassCombineData* RenderData = static_cast<const FinalPassCombineData*>(data);
		auto FinalPassShader = InGDI->GetShader(RenderData->ShaderId);
		auto RenderBuffer = InGDI->GetRenderBuffer(RenderData->RenderBufferId);

		ScopedGfxBind<GfxShader> Bind(FinalPassShader);
		{
			InGDI->ActivateTextureSlot(0);

			RenderBuffer->BindColorTarget(0);
			FinalPassShader->SetInt("screenTexture", 0);
			FinalPassShader->SetFloat("gamma", RenderData->Gamma);

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			RenderBuffer->UnbindColorTarget(0);
		}
	}

	DAWN_API void Draw::SSAOComputePass(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const SSAOComputePassData* RenderData = static_cast<const SSAOComputePassData*>(data);
		auto ComputeShader = InGDI->GetShader(RenderData->ShaderId);
		auto GBuffer = InGDI->GetRenderBuffer(RenderData->GBufferId);
		auto NoiseTexture = InGDI->GetTexture(RenderData->NoiseTextureId);

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


			ComputeShader->SetInt("gPosition", 0);
			ComputeShader->SetInt("gNormal", 1);
			ComputeShader->SetInt("gAlbedo", 2);
			ComputeShader->SetInt("Noise", 3);
			ComputeShader->SetFloat("radius", RenderData->Radius);
			ComputeShader->SetFloat("bias", RenderData->Bias);
			ComputeShader->SetFloat("power", RenderData->Power);

			for (unsigned int i = 0; i < 64; ++i) {
				ComputeShader->SetVec3("samples[" + std::to_string(i) + "]", (*RenderData->SSAOKernelData)[i]);
			}
			
			ComputeShader->SetMat4("projection", RenderData->Projection);
			ComputeShader->SetMat4("view", RenderData->View);

			InGDI->DrawIndexed(RenderData->FSQuadVAOId);

			InGDI->ActivateTextureSlot(3);
			NoiseTexture->Unbind();

			InGDI->ActivateTextureSlot(2);
			GBuffer->UnbindColorTarget(2);

			InGDI->ActivateTextureSlot(1);
			GBuffer->UnbindColorTarget(1);

			InGDI->ActivateTextureSlot(0);
			GBuffer->UnbindColorTarget(0);
		}
	}
}