#pragma once
#include "inc_dx11.h"
#include "Core/GDI/Base/inc_gfx_types.h"
#include "Core/GDI/Base/GfxBuffer.h"

namespace Dawn
{
	class DX11Buffer : public GfxBuffer
	{
	public:
		DX11Buffer(const GfxResId& InId, GfxGDIPtr InGDI);
		virtual ~DX11Buffer();

		virtual void Create(const GfxBufferDesc& InDesc, const GfxBufferData& InData) override;
		ID3D11Buffer* ToD3DBuffer() const;
		virtual u32 GetStride() const override {
			return Stride;
		}
	private:
		ComPtr<ID3D11Buffer> Buffer;
		u32 Stride;
		u32 Offset;
	};
}