#include "stdafx.h"
#include "inc_dx11_utils.h"
#include "DX11Buffer.h"
#include "DX11GDI.h"

Dawn::DX11Buffer::DX11Buffer(const GfxResId& InId, GfxGDIPtr InGDI)
	: GfxBuffer(InId, InGDI), Buffer(nullptr), Stride(0), Offset(0)
{
	
}

Dawn::DX11Buffer::~DX11Buffer()
{
	Buffer = nullptr;
}


void Dawn::DX11Buffer::Create(const GfxBufferDesc& InDesc, const GfxBufferData& InData)
{
	auto* gdi = static_cast<DX11GDI*>(GDI.get());

	D3D11_BUFFER_DESC bufferDesc = ToDX11BufferDesc(InDesc);
	
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = InData.Data;

	HRESULT hr = gdi->GetD3D11Device()->CreateBuffer(&bufferDesc, &resourceData, &Buffer);
	D_ASSERT(SUCCEEDED(hr), "Couldn't create index buffer!");

	Stride = InDesc.StructureByteStride;
	Offset = 0;
}

ID3D11Buffer* Dawn::DX11Buffer::ToD3DBuffer() const
{
	D_ASSERT(Buffer != nullptr, "No buffer has been created this object is inconsistent.");
	return this->Buffer.Get();
} 