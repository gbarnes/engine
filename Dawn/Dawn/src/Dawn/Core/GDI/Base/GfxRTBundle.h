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
		void SetDephtStencilTarget(u32 InWidth, u32 InHeight, GfxFormat InFormat);

		GfxTextureView* GetColorTarget(i32 InIndex) const;
		GfxTextureView* GetDepthStencilTarget() const;

		inline bool HasDepthStencilTarget() const
		{
			return DepthStencilTarget.IsValid;
		}

		inline u32 GetRenderTargetCount() const
		{
			return CurrentRTIndex + 1;
		}

		inline void SetGDI(GfxGDIPtr InGDI)
		{
			GDI = InGDI;
		}

	private:
		u32 CurrentRTIndex;
		GfxGDIPtr GDI;
		GfxResId RenderTargets[7];
		GfxResId DepthStencilTarget;
	};
}