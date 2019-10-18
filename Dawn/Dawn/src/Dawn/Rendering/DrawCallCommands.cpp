#include "DrawCallCommands.h"

namespace Dawn
{
	const DrawDispatchFunction Draw::DrawIndexedData::DRAW_FUNCTION = &Draw::DrawIndexed;
	const DrawDispatchFunction Draw::DrawInstancedData::DRAW_FUNCTION = &Draw::DrawInstanced;
	const DrawDispatchFunction Draw::DrawVertexData::DRAW_FUNCTION = &Draw::DrawVertex;
	const DrawDispatchFunction Draw::ClearSceneData::DRAW_FUNCTION = &Draw::ClearScene;
	const DrawDispatchFunction Draw::SetViewportData::DRAW_FUNCTION = &Draw::SetViewport;
	const DrawDispatchFunction Draw::FinalPassCombineData::DRAW_FUNCTION = &Draw::CombineFinalPass;

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

	DAWN_API void Draw::ClearScene(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const ClearSceneData* RenderData = static_cast<const ClearSceneData*>(data);
		InGDI->SetClearColor(RenderData->ClearColor);
		InGDI->Clear();
	}

	DAWN_API void Draw::SetViewport(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		const SetViewportData* RenderData = static_cast<const SetViewportData*>(data);
		InGDI->SetViewport(0, 0, RenderData->Width, RenderData->Height);
	}

	DAWN_API void Draw::CombineFinalPass(GfxGDI* InGDI, GfxShader* InShader, const void* data)
	{
		// todo --- this might be better of using actual indexed draw call and a more
		//			generic approach since we for example have a command to clear.
		const FinalPassCombineData* RenderData = static_cast<const FinalPassCombineData*>(data);
		auto FinalPassShader = InGDI->GetShader(RenderData->ShaderId);
		auto RenderBuffer = InGDI->GetRenderBuffer(RenderData->RenderBufferId);
		FinalPassShader->Bind();
		InGDI->ActivateTextureSlot(0);
		RenderBuffer->BindColorTarget(0);
		FinalPassShader->SetInt("screenTexture", 0);
		InGDI->DrawIndexed(RenderData->FSQuadVAOId);
		RenderBuffer->UnbindColorTarget(0);
		FinalPassShader->Unbind();
		
	}
}