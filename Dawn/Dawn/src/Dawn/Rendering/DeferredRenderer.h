#pragma once
#include "Renderer.h"

namespace Dawn
{
	class GfxGDI;
	struct Camera;
	class GfxShader;
	class Application;

	enum RenderPassId : i32
	{
		ShadowMap = 0,
		Geometry = 1,
		Lighting = 2,
		SSAO = 3,
		SSAOBlur = 4,
		Final = 5,
		Num
	};

	class DAWN_API DeferredRenderer : public IRenderer
	{
	public:
		RenderStats Stats;
		GfxShader* CurrentShader;
		TransientData TransientData;
		PerFrameData PerFrameData;
		SSAOSettings SSAOSettings;
		ShadowMapSettings ShadowSettings;

		//void AllocateTransientData(Application* InApp) override;
		void CreatePasses(Application* InApp) override;
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override;
		void BeginFrame(GfxGDI* InGDI, Camera* InCamera) override;
		void Submit(Application* InApp) override;
		void EndFrame(GfxGDI* InGDI) override;
	};
}
