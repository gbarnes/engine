#include "GfxPrimitiveFactory.h"

namespace Dawn
{
	GfxVertexArray* GfxPrimitiveFactory::AllocateQuad(GfxGDI* InGDI, const vec2& UVCoords)
	{
		float GridVertices[4 * 5] = {
		-0.5f,  0.5f, 0.0f,		0.0f, 0.0f,
		0.5f,  0.5f,  0.0f,		UVCoords.x, 0.0f,
		0.5f, -0.5f,  0.0f,		UVCoords.x,  UVCoords.y,
		-0.5f, -0.5f, 0.0f,		0.0f, UVCoords.y
		};

		u32 GridIndices[6] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		GfxVertexArray* GridArray;
		InGDI->CreateVertexArray(&GridArray);

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "aPos" },
			{ GfxShaderDataType::Float2, "aUV0" }
		};

		GfxVertexBuffer* VertexBuffer;
		InGDI->CreateVertexBuffer(GridVertices, sizeof(GridVertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(VertexBuffer);

		GfxIndexBuffer* IndexBuffer;
		InGDI->CreateIndexBuffer(GridIndices, sizeof(GridIndices) / sizeof(u32), &IndexBuffer);

		GridArray->SetIndexBuffer(IndexBuffer);

		return GridArray;
	}

	GfxVertexArray* GfxPrimitiveFactory::AllocateLine(GfxGDI* InGDI)
	{
		GfxVertexArray* LineArray;
		InGDI->CreateVertexArray(&LineArray);

		float LineVertices[2 * 3] = {
			0, 0, 0, 
			0, 0, 1
		};

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "aPos" }
		};

		GfxVertexBuffer* VertexBuffer;
		InGDI->CreateVertexBuffer(LineVertices, sizeof(LineVertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		LineArray->AttachVertexBuffer(VertexBuffer);

		return LineArray;
	}
}