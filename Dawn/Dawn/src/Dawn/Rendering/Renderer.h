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
	// at the https://blog.molecular-matters.com and other 
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
		virtual void BeginFrame(GfxGDI* InGDI, World* InWorld) = 0;
		virtual void Submit(Application* InApp) = 0;
		virtual void EndFrame(GfxGDI* InGDI) = 0;
	};

	class DAWN_API NullRenderer : public IRenderer
	{
	public:
		void AllocateTransientData(Application* InApp) override {}
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override {}
		void BeginFrame(GfxGDI* InGDI, World* InWorld) override {}
		void Submit(Application* InApp) override {}
		void EndFrame(GfxGDI* InGDI) override {
			InGDI->Present();
		}
	};

	struct DAWN_API TransientData
	{
		GfxResId RenderBufferId = INVALID_HANDLE;
	};

	struct DAWN_API PerFrameData
	{
		RenderBucket<u64> GeometryBucket;
		RenderBucket<u64> FinalPassBucket;
	};

	class DAWN_API DeferredRenderer : public IRenderer
	{
	public:
		TransientData TransientData;
		PerFrameData PerFrameData;

		void AllocateTransientData(Application* InApp) override;
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override;
		void BeginFrame(GfxGDI* InGDI, World* InWorld) override;
		void Submit(Application* InApp) override;
		void EndFrame(GfxGDI* InGDI) override;
	};

}