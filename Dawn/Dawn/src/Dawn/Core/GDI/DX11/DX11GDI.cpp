#include "stdafx.h"
#include "DX11GDI.h"
#include "DX11RenderBuffer.h"
#include "DX11Shader.h"
#include "DX11VertexArrayObject.h"
#include "DX11Buffer.h"
#include "DX11Texture.h"
#include "DX11TextureView.h"
#include "DX11PipelineStateObject.h"
#include "DX11Sampler.h"
#include "DX11ShaderResourceCache.h"
#include "inc_dx11_utils.h"

bool Dawn::DX11GDI::Init(const AppSettings& InSettings)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 2;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = InSettings.Hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_ADAPTER_DESC adapter;
	ZeroMemory(&adapter, sizeof(DXGI_ADAPTER_DESC));
	
	UINT flags = 0;
	
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,								// Adapter
		D3D_DRIVER_TYPE_HARDWARE,				// Driver Type always Hardware!
		0,										// no need to set this
		flags,				// flags 
		nullptr,								// Which feature levels should be used
		0,										// 
		D3D11_SDK_VERSION, 
		&scd, 
		&SwapChain, 
		&Device, 
		nullptr, 
		&Context
	);

	if (FAILED(hr))
		return false;

#ifdef _DEBUG
	Device->QueryInterface(IID_PPV_ARGS(&Debug));
#endif
	
	SetViewport(0, 0, InSettings.Width, InSettings.Height);
	CreateBackbufferRT();
	return true;
}

void Dawn::DX11GDI::Resize(i32 InWidth, i32 InHeight)
{
	// todo (gb): gleich einbauen!
	ReturnTextureView(BackbufferId);
	Context->Flush();
	SwapChain->ResizeBuffers(
		2,
		InWidth,
		InHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);

	CreateBackbufferRT();
	SetViewport(0, 0, InWidth, InHeight);
}


void Dawn::DX11GDI::Present()
{
	SwapChain->Present(1, 0);
}

void Dawn::DX11GDI::Shutdown()
{
	// note(gb): this must be called since this will take care of clearing out all the objects
	GfxGDI::Shutdown();

	SwapChain->SetFullscreenState(FALSE, NULL);
	SwapChain = nullptr;
	Context = nullptr;
	Device = nullptr;

#ifdef _DEBUG
	Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
	Debug = nullptr;
#endif
}

void Dawn::DX11GDI::ActivateTextureSlot(u32 InIndex)
{

}

void Dawn::DX11GDI::DrawIndexed(const GfxResId& InVertexArrayId)
{
	DX11VertexArrayObject* vao = static_cast<DX11VertexArrayObject*>(GetVertexArrayObject(InVertexArrayId));
	
	ID3D11Buffer* ib = vao->GetD3D11IndexBuffer();
	ID3D11Buffer* vb = vao->GetD3D11VertexBuffers();
	Context->IASetIndexBuffer(ib, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	u32 offsets = 0;
	Context->IASetVertexBuffers(0, vao->GetVertexBufferCount(), &vb, vao->GetStrides(), &offsets);
	Context->DrawIndexed(vao->GetIndiceCount(), 0, 0);
}

void Dawn::DX11GDI::DrawArray(const GfxResId& VertexArrayId)
{

}

void Dawn::DX11GDI::DrawInstanced(const GfxResId& VertexArrayId, u32 InAmount)
{

}

void Dawn::DX11GDI::SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = float(InLeft);
	viewport.TopLeftY = float(InTop);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = InRight - InLeft;
	viewport.Height = InBottom - InTop;
	Context->RSSetViewports(1, &viewport);

	D3D11_RECT rect = {};
	rect.left = InLeft;
	rect.top = InTop;
	rect.right = InRight;
	rect.bottom = InBottom;
	Context->RSSetScissorRects(1, &rect);
}

void Dawn::DX11GDI::ClearWithColor(const vec4& InColor)
{
	auto* buffer = static_cast<DX11TextureView*>(GetTextureView(BackbufferId));
	auto* rt = static_cast<ID3D11RenderTargetView*>(buffer->GetD3DView());
	Context->OMSetRenderTargets(1, &rt, nullptr);

	D_ASSERT(buffer != nullptr, "Backbuffer not found!");
	Context->ClearRenderTargetView(static_cast<ID3D11RenderTargetView*>(buffer->GetD3DView()), &InColor[0]);
}

void Dawn::DX11GDI::Clear()
{

}

void Dawn::DX11GDI::SetPipelineState(const GfxResId& InId)
{
	auto CastedState = static_cast<DX11PipelineStateObject*>(GetPipelineState(InId));
	if (CastedState == CurrentPipelineState)
		return;

	CurrentPipelineState = CastedState;

	Context->RSSetState(CurrentPipelineState->GetD3D11RasterizerState());
	Context->OMSetDepthStencilState(CurrentPipelineState->GetD3D11DepthStencilState(), 1);
	// for now 0xffffffff and nullptr
	
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	Context->OMSetBlendState(CurrentPipelineState->GetD3D11BlendState(), blendFactor, sampleMask);
	Context->IASetInputLayout(CurrentPipelineState->GetD3D11InputLayout());

	// todo (gb): change this :D
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Dawn::DX11GDI::SetVAO(const GfxResId& InId)
{
	const auto* vao = static_cast<const DX11VertexArrayObject*>(GetVertexArrayObject(InId));
	
	auto* vbs = vao->GetD3D11VertexBuffers();
	Context->IASetVertexBuffers(0, vao->GetVertexBufferCount(), &vbs, vao->GetStrides(), 0);

	auto* ibs = vao->GetD3D11IndexBuffer();
	if (ibs != nullptr)
		Context->IASetIndexBuffer(ibs, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, 0);
}

void Dawn::DX11GDI::BindPipelineShaders()
{
	D_ASSERT(CurrentPipelineState != nullptr, "No Pipeline State Object bound!");
	
	auto& vsShader = CurrentPipelineState->GetDesc().VertexShaderId;
	if (vsShader.IsValid)
	{
		auto* dxShader = static_cast<DX11Shader*>(GetShader(vsShader));
		Context->VSSetShader(static_cast<ID3D11VertexShader*>(dxShader->GetD3D11Shader()), nullptr, 0);
	}

	auto& psShader = CurrentPipelineState->GetDesc().PixelShaderId;
	if (psShader.IsValid)
	{
		auto* dxShader = static_cast<DX11Shader*>(GetShader(psShader));
		Context->PSSetShader(static_cast<ID3D11PixelShader*>(dxShader->GetD3D11Shader()), nullptr, 0);
	}
}

void Dawn::DX11GDI::CommitShaderResources(const GfxResId& InPSOId)
{
	DX11PipelineStateObject* pso = static_cast<DX11PipelineStateObject*>(GetPipelineState(InPSOId));
	const Array<u8>& shaders = pso->GetBoundShaderTypes();
	for (u32 i = 0; i < shaders.Count(); ++i)
	{
		GfxShaderType type = GfxShaderType(shaders[i]);
		DX11ShaderResourceCache* cache = static_cast<DX11ShaderResourceCache*>(pso->GetResourceCache(type));
		
		if (type == GfxShaderType::Pixel)
		{
			Context->PSSetConstantBuffers(0, u32(cache->NumConstantBuffers()), cache->ToConstantBuffers());
		}

		if (type == GfxShaderType::Vertex)
		{
			Context->VSSetConstantBuffers(0, u32(cache->NumConstantBuffers()), cache->ToConstantBuffers());
		}
	}
}

Dawn::GfxResId Dawn::DX11GDI::CreateBuffer(const GfxBufferDesc& InDesc, const GfxBufferData& InData, GfxBuffer** OutBuffer)
{
	auto* slot = BufferPool.Request();
	Allocators::LinearAllocator* Arena = ShaderPool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11Buffer), __alignof(DX11Buffer), 0)) DX11Buffer(slot->GetId(), this->shared_from_this());
	slot->Element->Create(InDesc, InData);
	if (OutBuffer != nullptr)
		*OutBuffer = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreateVertexArrayObject(GfxVertexArrayObject** OutBuffer)
{
	auto* slot = VertexArrayObjectPool.Request();
	Allocators::LinearAllocator* Arena = ShaderPool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11VertexArrayObject), __alignof(DX11VertexArrayObject), 0)) DX11VertexArrayObject(slot->GetId(), this->shared_from_this());

	if (OutBuffer != nullptr)
		*OutBuffer = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreateShader(const GfxShaderDesc& InDesc, const GfxShaderData& InData, Dawn::GfxShader** OutShader)
{
	auto* slot = ShaderPool.Request();
	Allocators::LinearAllocator* Arena = ShaderPool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11Shader), __alignof(DX11Shader), 0)) DX11Shader(slot->GetId(), this->shared_from_this());
	slot->Element->Create(InDesc, InData);
	if (OutShader != nullptr)
		*OutShader = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreateTexture(const Dawn::GfxTextureDesc& InDesc, const Dawn::GfxTextureData& InData, Dawn::GfxTexture** OutTexture)
{
	auto* slot = TexturePool.Request();
	Allocators::LinearAllocator* Arena = TexturePool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11Texture2D), __alignof(DX11Texture2D), 0)) DX11Texture2D(slot->GetId(), this->shared_from_this());
	slot->Element->Create(InDesc, InData);

	if (OutTexture != nullptr)
		*OutTexture = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreateTextureView(const Dawn::GfxTextureViewDesc& InDesc, Dawn::GfxTextureView** OutTextureView)
{
	auto* slot = TextureViewPool.Request();
	Allocators::LinearAllocator* Arena = TextureViewPool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11TextureView), __alignof(DX11TextureView), 0)) DX11TextureView(slot->GetId(), this->shared_from_this());
	slot->Element->Create(InDesc);

	if (OutTextureView != nullptr)
		*OutTextureView = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreateSampler(const GfxSamplerDesc& InDesc, GfxSampler** OutTexture)
{
	auto* slot = SamplerPool.Request();
	Allocators::LinearAllocator* Arena = SamplerPool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11Sampler), __alignof(DX11Sampler), 0)) DX11Sampler(slot->GetId(), this->shared_from_this());
	slot->Element->Create(InDesc);

	if (OutTexture != nullptr)
		*OutTexture = slot->Element;

	return slot->GetId();
}

Dawn::GfxResId Dawn::DX11GDI::CreatePSO(const GfxPipelineStateObjectDesc& InDesc, GfxPipelineStateObject** OutPSO) 
{
	auto* slot = PipelineStatePool.Request();
	Allocators::LinearAllocator* Arena = PipelineStatePool.GetMemArena();
	slot->Element = new (Arena->Allocate(sizeof(DX11PipelineStateObject), __alignof(DX11PipelineStateObject), 0)) DX11PipelineStateObject(InDesc, slot->GetId(), this->shared_from_this());
	slot->Element->Create();

	if (OutPSO != nullptr)
		*OutPSO = slot->Element;

	return slot->GetId();
}

void Dawn::DX11GDI::CreateBackbufferRT()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	GfxTextureViewDesc desc = {};
	desc.AdressToGPU = backBuffer.Get();
	desc.Type = GfxTextureViewType::RenderTargetView;

	GfxTextureView* view;
	BackbufferId = CreateTextureView(desc, &view);
	view->SetName("Backbuffer RenderTarget");
	
	auto* rt = static_cast<ID3D11RenderTargetView*>(static_cast<DX11TextureView*>(view)->GetD3DView());
	Context->OMSetRenderTargets(1, &rt, nullptr);

	backBuffer = nullptr;
}