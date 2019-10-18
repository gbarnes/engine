#include "GfxPrimitiveFactory.h"

namespace Dawn
{
	GfxVertexArray* GfxPrimitiveFactory::AllocateQuad(GfxGDI* InGDI, const vec2& UVCoords, float InSize)
	{
		float GridVertices[4 * 5] = {
		-InSize,  InSize, 0.0f,		0.0f, 0.0f,
		InSize,  InSize,  0.0f,		UVCoords.x, 0.0f,
		InSize, -InSize,  0.0f,		UVCoords.x,  UVCoords.y,
		-InSize, -InSize, 0.0f,		0.0f, UVCoords.y
		};

		u32 GridIndices[6] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		GfxVertexArray* GridArray;
		InGDI->CreateVertexArray(&GridArray);

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "position" },
			{ GfxShaderDataType::Float2, "uv0" }
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

	GfxVertexArray* GfxPrimitiveFactory::AllocateCube(GfxGDI* InGDI)
	{
		u32 indices[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3
		};

		float vertices[] = {
			// front
			-0.5, -0.5, 0.5,	0.0, 0.0,
			 0.5, -0.5, 0.5,	1.0, 0.0,
			 0.5, 0.5, 0.5,		1.0, 1.0,
			-0.5, 0.5, 0.5,		0.0, 1.0,
			// back
			-0.5, -0.5, -0.5,	0.0, 0.0,
			 0.5, -0.5, -0.5,	1.0, 0.0,
			 0.5, 0.5, -0.5,	1.0, 1.0,
			-0.5, 0.5, -0.5,	0.0, 1.0
		};

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "position" },
			{ GfxShaderDataType::Float3, "normal" },
			{ GfxShaderDataType::Float2, "uv0" }
		};

		GfxVertexArray* GridArray;
		InGDI->CreateVertexArray(&GridArray);

		GfxVertexBuffer* VertexBuffer;
		InGDI->CreateVertexBuffer(vertices, sizeof(vertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(VertexBuffer);

		GfxIndexBuffer* IndexBuffer;
		InGDI->CreateIndexBuffer(indices, sizeof(indices) / sizeof(u32), &IndexBuffer);

		GridArray->SetIndexBuffer(IndexBuffer);

		return GridArray;

	}
}