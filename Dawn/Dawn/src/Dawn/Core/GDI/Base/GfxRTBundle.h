#pragma once
#include "inc_gfx_types.h"
#include "GfxGDI.h"

namespace Dawn
{
	class GfxTextureView;

	class GfxRTBundle
	{
	public:
		GfxRTBundle();
		GfxRTBundle(GfxGDIPtr InGDI);
		~GfxRTBundle();

		void AttachColorTarget(u32 InWidth, u32 InHeight, GfxFormat InFormat);
		void SetDepthStencilTarget(u32 InWidth, u32 InHeight);

		GfxTextureView* GetColorTarget(i32 InIndex) const;
		GfxTextureView* GetDepthStencilTarget() const;

		GfxTextureView* GetRenderTargetShaderView(i32 InIndex) const;
		GfxTextureView* GetDepthStencilShaderView() const;

		inline GfxResId GetRenderTargetId(i32 InIndex) const
		{
			return RenderTargets[InIndex];
		}

		inline GfxResId GetDepthStencilTargetId() const
		{
			return DepthStencilTarget;
		}

		inline bool HasDepthStencilTarget() const
		{
			return DepthStencilTarget.IsValid;
		}

		inline u32 GetRenderTargetCount() const
		{
			return CurrentRTIndex;
		}

		inline void SetGDI(GfxGDIPtr InGDI)
		{
			GDI = InGDI;
		}

	private:
		u32 CurrentRTIndex;
		GfxGDIPtr GDI;
		GfxResId RenderTargets[7];
		GfxResId ShaderViews[7];
		GfxResId DepthStencilTarget;
		GfxResId DSShaderView;
	};
}