#include "stdafx.h"
#include "DX11VertexArrayObject.h"
#include "DX11Buffer.h"

Dawn::DX11VertexArrayObject::DX11VertexArrayObject(const GfxResId& InId, GfxGDIPtr InGDI)
	: GfxVertexArrayObject(InId, InGDI)
{
}

Dawn::DX11VertexArrayObject::~DX11VertexArrayObject()
{
	IndexBuffer->Release();

	for (i32 i = 0; i < VertexBuffers.Count(); ++i)
		VertexBuffers[i]->Release();

	Vertices.Clear();
}


void Dawn::DX11VertexArrayObject::AttachVertexBuffer(GfxBuffer* InBuffer)
{
	DX11Buffer* casted = static_cast<DX11Buffer*>(InBuffer);
	Vertices.Push(casted->GetId());

	ID3D11Buffer* buffer = casted->ToD3DBuffer();
	buffer->AddRef();
	VertexBuffers.Push(buffer);

	Strides.Push(InBuffer->GetStride());
	//Offset.Push(InBuffer->GetOffset());
}

void Dawn::DX11VertexArrayObject::SetIndexBuffer(GfxBuffer* InBuffer)
{
	IndexBuffer = static_cast<DX11Buffer*>(InBuffer)->ToD3DBuffer();

	D3D11_BUFFER_DESC desc;
	IndexBuffer->GetDesc(&desc);
	NumIndices = desc.ByteWidth / sizeof(u32);
	IndexBuffer->AddRef();
}


/*
Dawn::GfxVertexBuffer* Dawn::DX11VertexArray::GetVertexBuffer(GfxGDI* InGDI, u32 InIndex)
{
	return InGDI->GetVertexBuffer(Vertices[InIndex]);
}

Dawn::GfxIndexBuffer* Dawn::DX11VertexArray::GetIndexBuffer(GfxGDI* InGDI)
{
	return Indices;
}*/