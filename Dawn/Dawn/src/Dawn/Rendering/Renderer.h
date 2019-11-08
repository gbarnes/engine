#pragma once

#include "inc_common.h"
#include "Core/Math.h"
#include "Core/GDI/inc_gfx.h"
#include "RenderBucket.h"
#include "DrawCallCommands.h"

namespace Dawn
{
	class Application;
	class World;

	//
	// The rendering architecture is inspired by the blog posts 
	// at the https://blog.molecular-matters.com blog and other 
	// stateless rendering resources.
	//

	// 
	// Design Idea:
	// note-- maybe we just have one method (process or render which sorts -> submits -> presents -> clears buckets) ... meshes are queued before process
	// Per Frame Memory is allocated by the renderer
	// Meshes are converted into draw calls and put into respective Buckets (on multiple threads)(method needs to be added!)
	// DrawCalls are sorted on multiple threads per bucket 
	// DrawCalls are submitted to the gpu and rendered (one thread)
	// GDI Presents the screen
	// Per Frame Memory is cleared.
	//
	class DAWN_API IRenderer
	{
	public:
		virtual void AllocateTransientData(Application* InApp) = 0;
		virtual void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) = 0;
		virtual void BeginFrame(GfxGDI* InGDI, Camera* InCamera) = 0;
		virtual void Submit(Application* InApp) = 0;
		virtual void EndFrame(GfxGDI* InGDI) = 0;
	};

	class DAWN_API NullRenderer : public IRenderer
	{
	public:
		void AllocateTransientData(Application* InApp) override {}
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override {}
		void BeginFrame(GfxGDI* InGDI, Camera* InCamera) override {}
		void Submit(Application* InApp) override {}
		void EndFrame(GfxGDI* InGDI) override {
			InGDI->Present();
		}
	};

	struct DAWN_API TransientData
	{
		GfxResId GBufferId = INVALID_HANDLE;
		GfxResId FinalBufferId = INVALID_HANDLE;
		GfxResId SSAOBufferId = INVALID_HANDLE;
		GfxResId SSAOBlurBufferId = INVALID_HANDLE;
		GfxResId ShadowMapBufferId = INVALID_HANDLE;
	};

	struct Camera;
	struct DAWN_API PerFrameData
	{
		RenderBucket<u8> ShadowBucket;
		RenderBucket<u64> GeometryBucket;
		RenderBucket<u8> SSAOBucket;
		RenderBucket<u8> LightingBucket;
		RenderBucket<u8> FinalPassBucket;
		Camera* Camera;
	};

	struct DAWN_API SSAOSettings
	{
		bool bIsActive = true;
		float Radius = 0.55f;
		float Bias = 0.025f;
		float Power = 0.25f;
		GfxResId NoiseTextureId = INVALID_HANDLE;
		std::vector<vec3> Kernel;
	};

	struct DAWN_API ShadowMapSettings
	{
		u32 Width = 1024;
		u32 Height = 1024;
		float Bias = 0.005f;
	};


	class DAWN_API DeferredRenderer : public IRenderer
	{
	public:
		std::function<void(GfxGDI*, DeferredRenderer*)> OnPostRender;


		GfxShader* CurrentShader;
		TransientData TransientData;
		PerFrameData PerFrameData;
		SSAOSettings SSAOSettings;
		ShadowMapSettings ShadowSettings;

		void AllocateTransientData(Application* InApp) override;
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override;
		void BeginFrame(GfxGDI* InGDI, Camera* InCamera) override;
		void Submit(Application* InApp) override;
		void EndFrame(GfxGDI* InGDI) override;
	};

}
