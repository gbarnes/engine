#pragma once

#include "inc_common.h"
#include "Core/Math.h"
#include "Core/GDI/inc_gfx.h"
#include "Core/GDI/Base/GfxRTBundle.h"
#include "RenderBucket.h"
#include "Core/Memory/MemoryAllocators.h"
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

	struct RenderPass
	{
		RenderPass(const char* InName)
			: Name(InName)
		{

		}

		const char* Name;
		bool IsActive = true;
		GfxRTBundle RenderTargets;
		RenderBucket<u64> Bucket;
		std::function<void(RenderPass*)> Setup;
		std::function<void(RenderPass*)> PerFrameSetup;
		std::function<void(RenderPass*)> EndFrame;
	};

	// 
	// Design Idea:
	// note-- maybe we just have one method (process or render which sorts -> submits -> presents -> clears buckets) ... meshes are queued before process
	// Per Frame Memory is allocated by the renderer
	// Meshes are converted into draw calls and put into respective Buckets (on multiple threads)(method needs to be added!)
	// DrawCalls are sorted on multiple threads per bucket 
	// DrawCalls are submitted to the gpu and rendered (one thread)
	// GDI Presents the screen
	// Per Frame Memory is cleared.

	// https://kosmonautblog.wordpress.com/ great resource
	// http://svenandersson.se/2014/realtime-rendering-blogs.html more resources
	// https://seanmiddleditch.com/
	// https://bell0bytes.eu/game-programming/
	// https://bell0bytes.eu/inputsystem/
	// https://www.reddit.com/r/gamedev/
	class DAWN_API IRenderer
	{
	public:
		std::function<void(GfxGDI*, IRenderer*)> OnPostRender;

		IRenderer()
		{
			HeapMem = Memory::HeapArea(_64KB);
			PassAllocator = Allocators::LinearAllocator(HeapMem.GetStart(), HeapMem.GetEnd());
		}

		virtual ~IRenderer()
		{
			PassAllocator.Reset();
			HeapMem.Free();
		}

	//	virtual void AllocateTransientData(Application* InApp) = 0;
		virtual void CreatePasses(Application* InApp) = 0;
		virtual void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) = 0;
		virtual void BeginFrame(GfxGDI* InGDI, Camera* InCamera) = 0;
		virtual void Submit(Application* InApp) = 0;
		virtual void EndFrame(GfxGDI* InGDI) = 0;

		RenderPass* BeginPass(const char* InName, i32 InMaxSize)
		{
			auto* pass = new (PassAllocator.Allocate(sizeof(RenderPass), __alignof(RenderPass), 0)) RenderPass(InName);
			pass->Bucket.Allocate(InMaxSize);

			return pass;
		}

		void PushPass(RenderPass* InPass)
		{
			if(InPass->Setup != nullptr)
				InPass->Setup(InPass);

			Passes.push_back(InPass);
		}

		RenderPass* GetPass(i32 InIndex) 
		{
			return Passes[InIndex];
		}
	protected:
		Memory::HeapArea HeapMem;
		Allocators::LinearAllocator PassAllocator;
		std::vector<RenderPass*> Passes;
	};

	class DAWN_API NullRenderer : public IRenderer
	{
	public:
		void CreatePasses(Application* InApp) override {}
		void Resize(GfxGDI* InGDI, u32 InWidth, u32 InHeight) override {}
		void BeginFrame(GfxGDI* InGDI, Camera* InCamera) override {}
		void Submit(Application* InApp) override {}
		void EndFrame(GfxGDI* InGDI) override {
			InGDI->Present();
		}
	};

	struct DAWN_API TransientData
	{
		GfxRTBundle GBuffer;
		GfxRTBundle SSAOBuffer;
		GfxRTBundle SSAOBlurBuffer;
		GfxRTBundle ShadowMapBuffer;
		GfxRTBundle FinalBuffer;

		GfxResId GBufferId = INVALID_HANDLE;
		GfxResId FinalBufferId = INVALID_HANDLE;
		GfxResId SSAOBufferId = INVALID_HANDLE;
		GfxResId SSAOBlurBufferId = INVALID_HANDLE;
		GfxResId ShadowMapBufferId = INVALID_HANDLE;

		GfxResId PerAppBuffer;
		GfxResId PerFrameBuffer;
		GfxResId PerObjectBuffer;

		GfxResId QuadVAOId;
	};

	struct Camera;
	struct DAWN_API PerFrameData
	{
		Camera* Camera;
	};

	struct DAWN_API RenderStats
	{
		u32 DrawCalls;
		u64 TextureMemory;
		u64 VerticeCount;
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


}
