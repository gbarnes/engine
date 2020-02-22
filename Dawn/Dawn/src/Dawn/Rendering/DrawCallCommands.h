#pragma once
#include "inc_common.h"
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	class DeferredRenderer;

	typedef void(*DrawDispatchFunction)(GfxGDI*, DeferredRenderer*, const void*);

	namespace Draw
	{
		struct DAWN_API DrawIndexedData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			u32 IndexCount;
			GfxResId VertexArrayId;
			mat4 Model;
		};

		struct DAWN_API DrawInstancedData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			u32 IndexCount;
			GfxResId VertexArrayId;
			u32 Amount;
		};

		struct DAWN_API DrawVertexData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId VertexArrayId;
			mat4 Model;
		};

		struct DAWN_API ClearSceneData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
		};

		struct DAWN_API SetStateData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			//GfxState State;
		};

		struct DAWN_API ClearSceneWithColorData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			vec4 ClearColor;
		};

		struct DAWN_API SetViewportData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			u32 Width;
			u32 Height;
		};

		struct DAWN_API FXAAData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId RenderBufferId;
			GfxResId FSQuadVAOId;
		};

		struct DAWN_API LightingPassData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId GBufferId;
			GfxResId FSQuadVAOId;
			GfxResId SSAOBufferId;
			vec3 ViewPosition;
		};
		

		struct DAWN_API SSAOComputePassData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId GBufferId;
			GfxResId NoiseTextureId;
			GfxResId FSQuadVAOId;
			float Bias;
			float Radius;
			float Power;
			std::vector<vec3>* SSAOKernelData;
		};

		struct DAWN_API SSAOBlurPassData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId SSAOBlurBufferId;
			GfxResId SSAOBufferId;
			GfxResId FSQuadVAOId;
		};

		struct DAWN_API ShadowPassData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			u32 Width;
			u32 Height;
			mat4 LightSpace;
		};

		DAWN_API void DrawIndexed(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void DrawInstanced(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void DrawVertex(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void Clear(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void ClearWithColor(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void SetViewport(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void SSAOComputePass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void SSAOBlurPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void FXAAPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void LightingPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void ShadowPass(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
		DAWN_API void SetState(GfxGDI* InGDI, DeferredRenderer* InRenderer, const void* data);
	}

	//static_assert(std::is_pod<DrawIndexed>::value == true, "Draw must be a POD.");
	
}