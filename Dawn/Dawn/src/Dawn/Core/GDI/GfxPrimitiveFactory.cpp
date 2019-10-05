#include "GfxPrimitiveFactory.h"

namespace Dawn
{
	GfxVertexArray* GfxPrimitiveFactory::AllocateGrid()
	{
		float GridVertices[4 * 5] = {
		-0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
		0.5f,  0.5f,  0.0f, 1000.0f, 0.0f,
		0.5f, -0.5f,  0.0f, 1000.0f, 1000.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1000.0f
		};

		u32 GridIndices[6] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		auto GDI = GfxGDI::Get();

		GfxVertexArray* GridArray;
		GDI->CreateVertexArray(&GridArray);

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "aPos" },
			{ GfxShaderDataType::Float2, "aUV0" }
		};

		GfxVertexBuffer* VertexBuffer;
		GDI->CreateVertexBuffer(GridVertices, sizeof(GridVertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(VertexBuffer);

		GfxIndexBuffer* IndexBuffer;
		GDI->CreateIndexBuffer(GridIndices, sizeof(GridIndices) / sizeof(u32), &IndexBuffer);

		GridArray->SetIndexBuffer(IndexBuffer);

		return GridArray;
	}

	GfxVertexArray* GfxPrimitiveFactory::AllocateLine()
	{
		auto GDI = GfxGDI::Get();
		GfxVertexArray* LineArray;
		GDI->CreateVertexArray(&LineArray);

		float LineVertices[2 * 3] = {
			0, 0, 0, 
			0, 0, 1
		};

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "aPos" }
		};

		GfxVertexBuffer* VertexBuffer;
		GDI->CreateVertexBuffer(LineVertices, sizeof(LineVertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		LineArray->AttachVertexBuffer(VertexBuffer);

		return LineArray;
	}
}