#pragma once
#include "inc_common.h"
#include "Core/GDI/inc_gfx.h"

namespace Dawn
{
	typedef void(*DrawDispatchFunction)(GfxGDI*, GfxShader*, const void*);

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

		struct DAWN_API FinalPassCombineData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId RenderBufferId;
			GfxResId FSQuadVAOId;
			float Gamma = 2.2f;
		};

		struct DAWN_API LightingPassData
		{
			static const DrawDispatchFunction DRAW_FUNCTION;
			GfxResId ShaderId;
			GfxResId RenderBufferId;
			GfxResId FSQuadVAOId;
			vec3 ViewPosition;
			vec3 LightPos;
			vec4 LightColor;
			float LightIntensity;
		};

		DAWN_API void DrawIndexed(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void DrawInstanced(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void DrawVertex(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void Clear(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void ClearWithColor(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void SetViewport(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void CombineFinalPass(GfxGDI* InGDI, GfxShader* InShader, const void* data);
		DAWN_API void LightingPass(GfxGDI* InGDI, GfxShader* InShader, const void* data);
	}

	//static_assert(std::is_pod<DrawIndexed>::value == true, "Draw must be a POD.");
	
}