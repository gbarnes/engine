#pragma once
#include "GfxResource.h"
#include "inc_gfx_types.h"

namespace Dawn
{
	class GfxBuffer;

	class GfxVertexArrayObject : public GfxResource
	{
	public:
		GfxVertexArrayObject(const GfxResId& InId, GfxGDIPtr InGDI)
			: GfxResource(InId, InGDI) {}

		virtual void AttachVertexBuffer(GfxBuffer* InBuffer) = 0;
		virtual void SetIndexBuffer(GfxBuffer* InBuffer) = 0;
		virtual i32 GetIndiceCount() const = 0;
	};

	struct GfxVAOCreationData
	{
		struct
		{
			void* Data;
			u32 Num;			// How many Indices are there?
			u32 Size;			// How large is the entire buffer?
			u32 StrideSize;		// How large is a stride (normally sizeof(u32)
		} IndexBuffer;
		struct
		{
			void* Data;
			u32 Num;			// How many Vertices are there?
			u32 Size;			// How big is the entire buffer?
			u32 StrideSize;		// how large is one stride in the buffer (one structure containing all vb data position, normal etc.)
		} VertexBuffer;
	};

	inline GfxVertexArrayObject* CreateVAO(GfxGDIPtr InGDI, const GfxVAOCreationData& InData)
	{
		GfxVertexArrayObject* vao = nullptr;
		auto vaoID = InGDI->CreateVertexArrayObject(&vao);

		GfxBufferDesc vertexDesc = {};
		vertexDesc.BindFlags = Bind_VertexBuffer;
		vertexDesc.ByteWidth = InData.VertexBuffer.Size;
		vertexDesc.StructureByteStride = InData.VertexBuffer.StrideSize;
		vertexDesc.Usage = Usage_Default;
		vertexDesc.AccessFlags = CpuAccess_None;

		GfxBufferData vertexData = {};
		vertexData.Data = InData.VertexBuffer.Data;
		vertexData.Size = InData.VertexBuffer.Size;

		GfxBuffer* vertexBuffer;
		InGDI->CreateBuffer(vertexDesc, vertexData, &vertexBuffer);
		vao->AttachVertexBuffer(vertexBuffer);

		GfxBufferDesc indexDesc = {};
		indexDesc.BindFlags = Bind_IndexBuffer;
		indexDesc.ByteWidth = InData.IndexBuffer.Size;
		indexDesc.StructureByteStride = InData.IndexBuffer.StrideSize;
		indexDesc.Usage = Usage_Default;
		indexDesc.AccessFlags = CpuAccess_None;

		GfxBufferData indexData = {};
		indexData.Data = InData.IndexBuffer.Data;
		indexData.Size = InData.IndexBuffer.Size;

		GfxBuffer* indexBuffer;
		InGDI->CreateBuffer(indexDesc, indexData, &indexBuffer);
		vao->SetIndexBuffer(indexBuffer);
		return vao;
	}
}