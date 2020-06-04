#include "stdafx.h"
#include "GfxPrimitiveFactory.h"
#include "Base/GfxBuffer.h"
#include "Base/GfxVertexArrayObject.h"

namespace Dawn
{
	GfxVertexArrayObject* GfxPrimitiveFactory::AllocateQuad(GfxGDI* InGDI, const vec2& UVCoords, float InSize)
	{
		float QuadVertices[4 * 5] = {
			-InSize,  InSize, 0.0f,		0.0f, 0.0f,
			InSize,  InSize,  0.0f,		UVCoords.x, 0.0f,
			InSize, -InSize,  0.0f,		UVCoords.x,  UVCoords.y,
			-InSize, -InSize, 0.0f,		0.0f, UVCoords.y
		};

		u32 QuadIndices[6] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		GfxVertexArrayObject* VAO;
		InGDI->CreateVertexArrayObject(&VAO);

		/*GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "position" },
			{ GfxShaderDataType::Float2, "uv0" }
		};*/

		GfxBufferDesc vbDesc;
		vbDesc.AccessFlags = GfxCpuAccessFlags::CpuAccess_None;
		vbDesc.BindFlags = GfxBindFlags::Bind_VertexBuffer;
		vbDesc.ByteWidth = sizeof(QuadVertices);
		vbDesc.Usage = GfxUsageFlags::Usage_Default;
		vbDesc.StructureByteStride = sizeof(float) * 5;

		GfxBufferData vbData;
		vbData.Data = QuadVertices;
		vbData.Size = sizeof(QuadVertices);

		GfxBuffer* VertexBuffer;
		InGDI->CreateBuffer(vbDesc, vbData, &VertexBuffer);
		VAO->AttachVertexBuffer(VertexBuffer);

		GfxBufferDesc ibDesc;
		ibDesc.AccessFlags = GfxCpuAccessFlags::CpuAccess_None;
		ibDesc.BindFlags = GfxBindFlags::Bind_IndexBuffer;
		ibDesc.ByteWidth = sizeof(QuadIndices);
		ibDesc.Usage = GfxUsageFlags::Usage_Default;
		ibDesc.StructureByteStride = sizeof(u32);

		GfxBufferData ibData;
		ibData.Data = QuadIndices;
		ibData.Size = sizeof(QuadIndices);

		GfxBuffer* IndexBuffer;
		InGDI->CreateBuffer(ibDesc, ibData, &IndexBuffer);
		VAO->SetIndexBuffer(IndexBuffer);
		
		return VAO;
	}

	GfxVertexArrayObject* GfxPrimitiveFactory::AllocateLine(GfxGDI* InGDI)
	{
		float LineVertices[2 * 3] = {
			0, 0, 0, 
			0, 0, 1
		};

		GfxVertexArrayObject* VAO;
		InGDI->CreateVertexArrayObject(&VAO);

		/*GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "aPos" }
		};*/

		GfxBufferDesc vbDesc;
		vbDesc.AccessFlags = GfxCpuAccessFlags::CpuAccess_None;
		vbDesc.BindFlags = GfxBindFlags::Bind_VertexBuffer;
		vbDesc.ByteWidth = sizeof(LineVertices);
		vbDesc.Usage = GfxUsageFlags::Usage_Default;
		vbDesc.StructureByteStride = sizeof(float);

		GfxBufferData vbData;
		vbData.Data = &LineVertices[0];
		vbData.Size = sizeof(LineVertices);

		GfxBuffer* VertexBuffer;
		InGDI->CreateBuffer(vbDesc, vbData, &VertexBuffer);
		VAO->AttachVertexBuffer(VertexBuffer);

		return VAO;
	}

	GfxVertexArrayObject* GfxPrimitiveFactory::AllocateCube(GfxGDI* InGDI)
	{
		u32 indices[] = {
			1,1,0,
			3,2,0,
			6,5,4,
			7,6,4,
			10,9,8,
			11,10,8,
			14,13,12,
			15,14,12,
			18,17,16,
			19,18,16,
			22,21,20,
			23,22,20

		};

		float vertices[] = {
			-0.500000,-0.500000,0.500000,	0.000000,0.000000,1.000000,    0.375000,0.000000,	0.000000,0.000000,
			0.500000,-0.500000,0.500000,	0.000000,0.000000,1.000000,	   0.625000,0.000000,	0.000000,0.000000,
			0.500000,0.500000,0.500000,		0.000000,0.000000,1.000000,	   0.625000,0.250000,	0.000000,0.000000,
			-0.500000,0.500000,0.500000,	0.000000,0.000000,1.000000,	   0.375000,0.250000,	0.000000,0.000000,
			-0.500000,0.500000,0.500000,	0.000000,1.000000,0.000000,	   0.375000,0.250000,	0.000000,0.000000,
			0.500000,0.500000,0.500000,		0.000000,1.000000,0.000000,	   0.625000,0.250000,	0.000000,0.000000,
			0.500000,0.500000,-0.500000,	0.000000,1.000000,0.000000,	   0.625000,0.500000,	0.000000,0.000000,
			-0.500000,0.500000,-0.500000,	0.000000,1.000000,0.000000,	   0.375000,0.500000,	0.000000,0.000000,
			-0.500000,0.500000,-0.500000,	0.000000,0.000000,-1.000000,   0.375000,0.500000,	0.000000,0.000000,
			0.500000,0.500000,-0.500000,	0.000000,0.000000,-1.000000,   0.625000,0.500000,	0.000000,0.000000,
			0.500000,-0.500000,-0.500000,	0.000000,0.000000,-1.000000,   0.625000,0.750000,	0.000000,0.000000,
			-0.500000,-0.500000,-0.500000,	0.000000,0.000000,-1.000000,   0.375000,0.750000,	0.000000,0.000000,
			-0.500000,-0.500000,-0.500000,	0.000000,-1.000000,0.000000,   0.375000,0.750000,	0.000000,0.000000,
			0.500000,-0.500000,-0.500000,	0.000000,-1.000000,0.000000,   0.625000,0.750000,	0.000000,0.000000,
			0.500000,-0.500000,0.500000,	0.000000,-1.000000,0.000000,   0.625000,1.000000,	0.000000,0.000000,
			-0.500000,-0.500000,0.500000,	0.000000,-1.000000,0.000000,   0.375000,1.000000,	0.000000,0.000000,
			0.500000,-0.500000,0.500000,	1.000000,0.000000,0.000000,	   0.625000,0.000000,	0.000000,0.000000,
			0.500000,-0.500000,-0.500000,	1.000000,0.000000,0.000000,	   0.875000,0.000000,	0.000000,0.000000,
			0.500000,0.500000,-0.500000,	1.000000,0.000000,0.000000,	   0.875000,0.250000,	0.000000,0.000000,
			0.500000,0.500000,0.500000,		1.000000,0.000000,0.000000,	   0.625000,0.250000,	0.000000,0.000000,
			-0.500000,-0.500000,-0.500000,	-1.000000,0.000000,0.000000,   0.125000,0.000000,	0.000000,0.000000,
			-0.500000,-0.500000,0.500000,	-1.000000,0.000000,0.000000,   0.375000,0.000000,	0.000000,0.000000,
			-0.500000,0.500000,0.500000,	-1.000000,0.000000,0.000000,   0.375000,0.250000,	0.000000,0.000000,
			-0.500000,0.500000,-0.500000,	-1.000000,0.000000,0.000000,   0.125000,0.250000,	0.000000,0.000000
		};

		GfxVertexArrayObject* VAO;
		InGDI->CreateVertexArrayObject(&VAO);

		/*GfxBufferLayout Layout =
		{
			{ GfxShaderDataType::Float3, "position" },
			{ GfxShaderDataType::Float3, "normal" },
			{ GfxShaderDataType::Float2, "uv0" },
			{ GfxShaderDataType::Float2, "uv1" }
		};*/

		GfxBufferDesc vbDesc;
		vbDesc.AccessFlags = GfxCpuAccessFlags::CpuAccess_None;
		vbDesc.BindFlags = GfxBindFlags::Bind_VertexBuffer;
		vbDesc.ByteWidth = sizeof(vertices);
		vbDesc.Usage = GfxUsageFlags::Usage_Default;
		vbDesc.StructureByteStride = sizeof(float) * 10;

		GfxBufferData vbData;
		vbData.Data = &vertices[0];
		vbData.Size = sizeof(vertices);

		GfxBuffer* VertexBuffer;
		InGDI->CreateBuffer(vbDesc, vbData, &VertexBuffer);
		VAO->AttachVertexBuffer(VertexBuffer);

		GfxBufferDesc ibDesc;
		ibDesc.AccessFlags = GfxCpuAccessFlags::CpuAccess_None;
		ibDesc.BindFlags = GfxBindFlags::Bind_IndexBuffer;
		ibDesc.ByteWidth = sizeof(indices);
		ibDesc.Usage = GfxUsageFlags::Usage_Default;
		ibDesc.StructureByteStride = sizeof(u32);

		GfxBufferData ibData;
		ibData.Data = &indices[0];
		ibData.Size = sizeof(indices);

		GfxBuffer* IndexBuffer;
		InGDI->CreateBuffer(ibDesc, ibData, &IndexBuffer);
		VAO->SetIndexBuffer(IndexBuffer);

		return VAO;
	}
}