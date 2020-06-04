#include "stdafx.h"
#include "DX11GDI.h"
#include "DX11Shader.h"
#include "DX11VertexArrayObject.h"
#include "DX11Buffer.h"
#include "DX11Texture.h"
#include "DX11TextureView.h"
#include "DX11PipelineStateObject.h"
#include "DX11Sampler.h"
#include "DX11ShaderResourceCache.h"
#include "Brofiler.h"
#include "inc_dx11_utils.h"

bool Dawn::DX11GDI::Init(const AppSettings& InSettings)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = InSettings.Hwnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_ADAPTER_DESC adapter;
	ZeroMemory(&adapter, sizeof(DXGI_ADAPTER_DESC));
	
	UINT flags = 0;
	flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	//flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};


	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		nullptr,								// Adapter
		D3D_DRIVER_TYPE_HARDWARE,				// Driver Type always Hardware!
		0,										// no need to set this
		flags,				// flags 
		featureLevels,								// Which feature levels should be used
		_countof(featureLevels),									// 
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

Dawn::GfxResId Dawn::DX11GDI::GetCurrentPSOId()
{
	if (CurrentPipelineState == nullptr)
		return INVALID_HANDLE;

	return CurrentPipelineState->GetId();
}

void Dawn::DX11GDI::Present()
{
	BROFILER_CATEGORY("GDI_Present", Brofiler::Color::Red);
	SwapChain->Present(0, 0);
	Context->ClearState();
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
	BROFILER_CATEGORY("GDI_DrawIndexed", Brofiler::Color::Red);

	DX11VertexArrayObject* vao = static_cast<DX11VertexArrayObject*>(GetVertexArrayObject(InVertexArrayId));
	
	ID3D11Buffer* ib = vao->GetD3D11IndexBuffer();
	Context->IASetIndexBuffer(ib, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	u32 offsets = 0;
	Context->IASetVertexBuffers(0, vao->GetVertexBufferCount(), vao->GetD3D11VertexBuffers(), vao->GetStrides(), &offsets);
	Context->DrawIndexed(vao->GetIndiceCount(), 0, 0);
}

void Dawn::DX11GDI::DrawArray(const GfxResId& VertexArrayId)
{

}

void Dawn::DX11GDI::DrawInstanced(const GfxResId& VertexArrayId, u32 InAmount)
{
	BROFILER_CATEGORY("GDI_DrawInstanced", Brofiler::Color::Red);

	DX11VertexArrayObject* vao = static_cast<DX11VertexArrayObject*>(GetVertexArrayObject(VertexArrayId));

	ID3D11Buffer* ib = vao->GetD3D11IndexBuffer();
	Context->IASetIndexBuffer(ib, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	// todo(gb): pass this by the vao!
	u32 offsets[] = { 0, 0 };
	Context->IASetVertexBuffers(0, vao->GetVertexBufferCount(), vao->GetD3D11VertexBuffers(), vao->GetStrides(), offsets);
	Context->DrawIndexedInstanced(vao->GetIndiceCount(), InAmount, 0, 0, 0);
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

void Dawn::DX11GDI::SetRenderTargetBundle(const GfxRTBundle* InBundle)
{
	BROFILER_CATEGORY("GDI_SetRenderTargetBundle", Brofiler::Color::Red);

	// Todo(gb): can we speed this up?!
	Array<ID3D11RenderTargetView*> rts;
	for (u32 i = 0; i < InBundle->GetRenderTargetCount(); ++i)
	{
		rts.Push(static_cast<ID3D11RenderTargetView*>(static_cast<DX11TextureView*>(InBundle->GetColorTarget(i))->GetD3DView()));
	}

	ID3D11DepthStencilView* dsView = nullptr;
	if (InBundle->HasDepthStencilTarget())
		dsView = static_cast<ID3D11DepthStencilView*>(static_cast<DX11TextureView*>(InBundle->GetDepthStencilTarget())->GetD3DView());

	Context->OMSetRenderTargets(InBundle->GetRenderTargetCount(), rts.Data(), dsView);
}

void Dawn::DX11GDI::ClearWithColor(const GfxResId& InTextureViewId, const vec4& InColor)
{
	BROFILER_CATEGORY("GDI_ClearWithColor", Brofiler::Color::Red);

	auto* buffer = static_cast<DX11TextureView*>(GetTextureView(InTextureViewId));
	auto* rt = static_cast<ID3D11RenderTargetView*>(buffer->GetD3DView());

	D_ASSERT(buffer != nullptr, "RenderTarget not found!");
	Context->ClearRenderTargetView(rt, &InColor[0]);
}

void Dawn::DX11GDI::ClearDepthStencil(const GfxResId& InDepthStencilId, float InDepth, i32 InStencil)
{
	BROFILER_CATEGORY("GDI_ClearDepthStencil", Brofiler::Color::Red);

	auto* buffer = static_cast<DX11TextureView*>(GetTextureView(InDepthStencilId));
	auto* rt = static_cast<ID3D11DepthStencilView*>(buffer->GetD3DView());

	D_ASSERT(buffer != nullptr, "DepthStencil not found!");
	Context->ClearDepthStencilView(rt, D3D11_CLEAR_STENCIL | D3D11_CLEAR_DEPTH, InDepth, InStencil);
}

void Dawn::DX11GDI::SetToBackbuffer()
{
	BROFILER_CATEGORY("GDI_SetToBackbuffer", Brofiler::Color::Red);

	ID3D11RenderTargetView* rt = static_cast<ID3D11RenderTargetView*>(static_cast<DX11TextureView*>(GetTextureView(BackbufferId))->GetD3DView());
	D_ASSERT(rt != nullptr, "Backbuffer not found!");
	Context->OMSetRenderTargets(1, &rt, nullptr);
}

void Dawn::DX11GDI::Clear()
{
	Context->ClearState();
}

void Dawn::DX11GDI::SetPipelineState(const GfxResId& InId)
{
	BROFILER_CATEGORY("GDI_SetPipelineState", Brofiler::Color::Red);

	auto CastedState = static_cast<DX11PipelineStateObject*>(GetPipelineState(InId));
	if (CastedState == CurrentPipelineState)
		return;

	SetViewport(0, 0, 1280, 720);

	CurrentPipelineState = CastedState;

	Context->RSSetState(CurrentPipelineState->GetD3D11RasterizerState());
	Context->OMSetDepthStencilState(CurrentPipelineState->GetD3D11DepthStencilState(), 0);
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
	BROFILER_CATEGORY("GDI_SetVAO", Brofiler::Color::Red);
	auto* vao = static_cast<DX11VertexArrayObject*>(GetVertexArrayObject(InId));
	Context->IASetVertexBuffers(0, vao->GetVertexBufferCount(), vao->GetD3D11VertexBuffers(), vao->GetStrides(), 0);

	auto* ibs = vao->GetD3D11IndexBuffer();
	if (ibs != nullptr)
		Context->IASetIndexBuffer(ibs, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, 0);
}

void Dawn::DX11GDI::BindPipelineShaders()
{
	BROFILER_CATEGORY("GDI_BindPipelineShaders", Brofiler::Color::Red);

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
	BROFILER_CATEGORY("GDI_CommitShaderResources", Brofiler::Color::Red);

	DX11PipelineStateObject* pso = static_cast<DX11PipelineStateObject*>(GetPipelineState(InPSOId));
	const Array<u8>& shaders = pso->GetBoundShaderTypes();
	for (u32 i = 0; i < shaders.Count(); ++i)
	{
		GfxShaderType type = GfxShaderType(shaders[i]);
		DX11ShaderResourceCache* cache = static_cast<DX11ShaderResourceCache*>(pso->GetResourceCache(type));
		
		if (type == GfxShaderType::Pixel)
		{
			if (cache->NumConstantBuffers() > 0)
				Context->PSSetConstantBuffers(0, u32(cache->NumConstantBuffers()), cache->ToConstantBuffers());

			if (cache->NumSamplers() > 0)
				Context->PSSetSamplers(0, u32(cache->NumSamplers()), cache->ToSamplers());
			if(cache->NumShaderResourceViews() > 0)
				Context->PSSetShaderResources(0, u32(cache->NumShaderResourceViews()), cache->ToShaderResourceViews());
		}

		if (type == GfxShaderType::Vertex)
		{
			if (cache->NumConstantBuffers() > 0)
				Context->VSSetConstantBuffers(0, u32(cache->NumConstantBuffers()), cache->ToConstantBuffers());

			if (cache->NumSamplers() > 0)
				Context->VSSetSamplers(0, u32(cache->NumSamplers()), cache->ToSamplers());

			if (cache->NumShaderResourceViews() > 0)
				Context->VSSetShaderResources(0, u32(cache->NumShaderResourceViews()), cache->ToShaderResourceViews());
		}
	}
}

void Dawn::DX11GDI::ClearShaderResources()
{
	BROFILER_CATEGORY("GDI_ClearShaderResources", Brofiler::Color::Red);

	ID3D11ShaderResourceView* srvs[128] = { 0 };
	memset(srvs, 0, sizeof(srvs));
	Context->PSSetShaderResources(0, 128, srvs);
	Context->VSSetShaderResources(0, 128, srvs);

	//ID3D11Buffer* buffers[5] = { 0 };
	//memset(buffers, 0, sizeof(buffers));
	//Context->PSSetConstantBuffers(0, 5, buffers);
	//Context->VSSetConstantBuffers(0, 5, buffers);

	/*ID3D11SamplerState* samplers[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = { 0 };
	memset(samplers, 0, sizeof(samplers));
	Context->PSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, samplers);
	Context->VSSetSamplers(0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT, samplers);*/

}

void Dawn::DX11GDI::UpdateConstantBuffer(const GfxResId& InBufferId, void* InData, i32 InSize)
{
	BROFILER_CATEGORY("GDI_UpdateConstantBuffer", Brofiler::Color::Red);

	// todo (gb): should this method be written to take multiple buffers??
	DX11Buffer* buffer = static_cast<DX11Buffer*>(GetBuffer(InBufferId));
	D_ASSERT(buffer != nullptr, "The constant buffer couldn't be found!");

	D3D11_MAPPED_SUBRESOURCE mappedRes = {};
	Context->Map(buffer->ToD3DBuffer(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedRes);
	memcpy(mappedRes.pData, InData, InSize);
	Context->Unmap(buffer->ToD3DBuffer(), 0);
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

	backBuffer = nullptr;
}