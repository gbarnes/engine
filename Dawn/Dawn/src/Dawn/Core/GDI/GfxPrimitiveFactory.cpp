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
			0,1,2,
			0,2,3,
			4,5,6,
			4,6,7,
			8,9,10,
			8,10,11,
			12,13,14,
			12,14,15,
			16,17,18,
			16,18,19,
			20,21,22,
			20,22,23

		};

		float vertices[] = {
			-0.500000,-0.500000,0.500000,		
			0.500000,-0.500000,0.500000,		
			0.500000,0.500000,0.500000,			
			-0.500000,0.500000,0.500000,		
			-0.500000,0.500000,0.500000,		
			0.500000,0.500000,0.500000,			
			0.500000,0.500000,-0.500000,		
			-0.500000,0.500000,-0.500000,		
			-0.500000,0.500000,-0.500000,		
			0.500000,0.500000,-0.500000,		
			0.500000,-0.500000,-0.500000,		
			-0.500000,-0.500000,-0.500000,		
			-0.500000,-0.500000,-0.500000,		
			0.500000,-0.500000,-0.500000,		
			0.500000,-0.500000,0.500000,		
			-0.500000,-0.500000,0.500000,		
			0.500000,-0.500000,0.500000,		
			0.500000,-0.500000,-0.500000,		
			0.500000,0.500000,-0.500000,		
			0.500000,0.500000,0.500000,			
			-0.500000,-0.500000,-0.500000,		
			-0.500000,-0.500000,0.500000,		
			-0.500000,0.500000,0.500000,		
			-0.500000,0.500000,-0.500000,		
		};

		float normals[] = {
			0.000000,0.000000,1.000000,
			0.000000,0.000000,1.000000,
			0.000000,0.000000,1.000000,
			0.000000,0.000000,1.000000,
			0.000000,1.000000,0.000000,
			0.000000,1.000000,0.000000,
			0.000000,1.000000,0.000000,
			0.000000,1.000000,0.000000,
			0.000000,0.000000,-1.000000,
			0.000000,0.000000,-1.000000,
			0.000000,0.000000,-1.000000,
			0.000000,0.000000,-1.000000,
			0.000000,-1.000000,0.000000,
			0.000000,-1.000000,0.000000,
			0.000000,-1.000000,0.000000,
			0.000000,-1.000000,0.000000,
			1.000000,0.000000,0.000000,
			1.000000,0.000000,0.000000,
			1.000000,0.000000,0.000000,
			1.000000,0.000000,0.000000,
			-1.000000,0.000000,0.000000,
			-1.000000,0.000000,0.000000,
			-1.000000,0.000000,0.000000,
			-1.000000,0.000000,0.000000
		};

		float textures[] = {
			0.375000,0.000000,	0.000000,0.000000,
			0.625000,0.000000,	0.000000,0.000000,
			0.625000,0.250000,	0.000000,0.000000,
			0.375000,0.250000,	0.000000,0.000000,
			0.375000,0.250000,	0.000000,0.000000,
			0.625000,0.250000,	0.000000,0.000000,
			0.625000,0.500000,	0.000000,0.000000,
			0.375000,0.500000,	0.000000,0.000000,
			0.375000,0.500000,	0.000000,0.000000,
			0.625000,0.500000,	0.000000,0.000000,
			0.625000,0.750000,	0.000000,0.000000,
			0.375000,0.750000,	0.000000,0.000000,
			0.375000,0.750000,	0.000000,0.000000,
			0.625000,0.750000,	0.000000,0.000000,
			0.625000,1.000000,	0.000000,0.000000,
			0.375000,1.000000,	0.000000,0.000000,
			0.625000,0.000000,	0.000000,0.000000,
			0.875000,0.000000,	0.000000,0.000000,
			0.875000,0.250000,	0.000000,0.000000,
			0.625000,0.250000,	0.000000,0.000000,
			0.125000,0.000000,	0.000000,0.000000,
			0.375000,0.000000,	0.000000,0.000000,
			0.375000,0.250000,	0.000000,0.000000,
			0.125000,0.250000,	0.000000,0.000000
		};

		GfxVertexArray* GridArray;
		InGDI->CreateVertexArray(&GridArray);

		GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "position" },
		};

		GfxVertexBuffer* VertexBuffer;
		InGDI->CreateVertexBuffer(vertices, sizeof(vertices), &VertexBuffer);
		VertexBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(VertexBuffer);

		Layout =
		{
			{ GfxShaderDataType::Float3, "normal" },
		};

		GfxVertexBuffer* NormalBuffer;
		InGDI->CreateVertexBuffer(normals, sizeof(normals), &NormalBuffer);
		NormalBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(NormalBuffer);

		Layout =
		{
			{ GfxShaderDataType::Float2, "uv0" },
			{ GfxShaderDataType::Float2, "uv1" }
		};

		GfxVertexBuffer* UvBuffer;
		InGDI->CreateVertexBuffer(textures, sizeof(textures), &UvBuffer);
		UvBuffer->SetLayout(Layout);
		GridArray->AttachVertexBuffer(UvBuffer);

		GfxIndexBuffer* IndexBuffer;
		InGDI->CreateIndexBuffer(indices, sizeof(indices) / sizeof(u32), &IndexBuffer);

		GridArray->SetIndexBuffer(IndexBuffer);

		return GridArray;

	}
}