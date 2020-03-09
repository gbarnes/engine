#pragma once
#include "inc_dx11.h"
#include "Core/Container/Array.h"
#include "Core/GDI/Base/GfxVertexArrayObject.h"
#include "Core/GDI/Base/GfxGDI.h"

namespace Dawn
{
	class GfxBuffer;

	class DX11VertexArrayObject : public GfxVertexArrayObject
	{
	public:
		DX11VertexArrayObject(const GfxResId& Id, GfxGDIPtr InGDI);
		virtual ~DX11VertexArrayObject();

		virtual void AttachVertexBuffer(GfxBuffer* InBuffer) override;
		virtual void SetIndexBuffer(GfxBuffer* InBuffer) override;

		i64 GetVertexBufferCount() const
		{
			return Vertices.Count();
		}

		ID3D11Buffer*const* GetD3D11VertexBuffers() const
		{
			return VertexBuffers.Data();
		}

		const u32* GetStrides() const
		{
			return Strides.Data();
		}

		virtual i32 GetIndiceCount() const override
		{
			return NumIndices;
		}


		ID3D11Buffer* GetD3D11IndexBuffer() const
		{
			return IndexBuffer;
		}

	private:
		Array<GfxResId> Vertices;

		// note (gb): mhm we shouldn't rely on direct references but we keep this for now!
		Array<ID3D11Buffer*> VertexBuffers;
		ID3D11Buffer* IndexBuffer;
		Array<u32> Strides;
		Array<u32> Offsets;
		u32 NumIndices;
	};
}