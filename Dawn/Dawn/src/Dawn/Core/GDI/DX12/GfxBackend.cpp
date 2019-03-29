#pragma once
#ifdef USE_DX12_GFX
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"
#include "GfxContext.h"
#include "GfxParallel.h"
#include "Application.h"
#include "GfxBackend.h"
#include "imgui.h"
#include "GfxQueue.h"
#include "GfxDescriptorAllocator.h"
#include "GfxResourceStateTracker.h"
#include "GfxRootSignature.h"
#include "GfxCmdList.h"
#include "GfxTexture.h"
#include "GfxRenderTarget.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"

namespace Dawn
{
	const u32 g_NumFrames = 3;

	// Device Variables
	static ComPtr<IDXGIAdapter3> g_Adapter;
	static ComPtr<ID3D12Device2> g_Device;
	static ComPtr<IDXGISwapChain3> g_SwapChain;

	// Backbuffer Variables
	static GfxRenderTarget g_RenderTarget;
	static GfxTexture g_BackBufferTextures[g_NumFrames];
	static u32 g_CurrentBackBufferIndex;

	// Command Queue Variables
	static std::shared_ptr<GfxQueue> g_DirectQueue;
	static std::shared_ptr<GfxQueue> g_ComputeQueue;
	static std::shared_ptr<GfxQueue> g_CopyQueue;
	static uint64_t g_FrameFenceValues[g_NumFrames] = {};
	static uint64_t g_FenceValue = 0;
	static bool g_Initialized = false;

	// Allocator Variables
	static std::unique_ptr<GfxDescriptorAllocator> g_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	// Root Signatures
	static std::vector<std::shared_ptr<GfxRootSignature>> g_RootSignatures;

	// Pipeline States
	static std::vector<ComPtr<ID3D12PipelineState>> g_PipelineStates;

	// Display Info
	static bool g_UseVsync = false;
	static u32 g_Width;
	static u32 g_Height;
	static D3D12_VIEWPORT g_Viewport;
	static D3D12_RECT g_ScissorRect;

	bool GfxBackend::IsInitialized()
	{
		return g_Initialized;
	}

	void UpdateRenderTargetViews()
	{
		for (int i = 0; i < g_NumFrames; ++i)
		{
			ComPtr<ID3D12Resource> backBuffer;
			ThrowIfFailed(g_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			GfxResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);

			g_BackBufferTextures[i].SetD3D12Resource(backBuffer);
			g_BackBufferTextures[i].CreateViews();
		}
	}

	EResult GfxBackend::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false)
	{
		g_UseVsync = InUseVSync;
		g_Width = InWidth;
		g_Height = InHeight;

		g_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(InWidth), static_cast<float>(InHeight));
		g_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

		g_Adapter = GfxContext::GetAdapter(false);
		if (g_Adapter == nullptr)
			return EResult_ERROR;

		g_Device = GfxContext::CreateDevice(g_Adapter);
		if (g_Device == nullptr)
			return EResult_ERROR;

		// allocate the queues
		{
			g_DirectQueue = std::make_shared<GfxQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
			g_ComputeQueue = std::make_shared<GfxQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
			g_CopyQueue = std::make_shared<GfxQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
		}

		g_SwapChain = GfxContext::CreateSwapChain(InHWnd, g_DirectQueue->GetD3D12CommandQueue(), InWidth, InHeight, g_NumFrames);
		if (g_SwapChain == nullptr)
			return EResult_ERROR;


		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			g_DescriptorAllocators[i] = std::make_unique<GfxDescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i), 1);
		}

		UpdateRenderTargetViews();

		g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

		g_Initialized = true;

		return EResult_OK;
	}

	//
	//
	//
	void GfxBackend::Shutdown()
	{
		if (!IsInitialized())
			return;

		Flush();

		for (auto rootSig : g_RootSignatures)
			rootSig.reset();

		g_RootSignatures.clear();

		for (auto pipelineState : g_PipelineStates)
			pipelineState.Reset();

		g_PipelineStates.clear();

		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
			g_DescriptorAllocators[i].reset();

		for (int i = 0; i < g_NumFrames; ++i)
			g_BackBufferTextures[i].Reset();

		{
			g_ComputeQueue.reset();
			g_CopyQueue.reset();
			g_DirectQueue.reset();
		}

		{
			g_SwapChain.Reset();
			g_Device.Reset();
			g_Adapter.Reset();
		}
	}

	void GfxBackend::Resize(u32 InWidth, u32 InHeight)
	{
		if (!IsInitialized())
			return;

		if (g_Width != InWidth || g_Height != InHeight)
		{
			g_Width = InWidth;
			g_Height = InHeight;
			g_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(InWidth), static_cast<float>(InHeight));

			Flush();

			g_RenderTarget.AttachTexture(Color0, GfxTexture());
			for (int i = 0; i < g_NumFrames; ++i)
			{
				GfxResourceStateTracker::RemoveGlobalResourceState(g_BackBufferTextures[i].GetD3D12Resource().Get());
				g_BackBufferTextures[i].Reset();
			}

			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			ThrowIfFailed(g_SwapChain->GetDesc(&swapChainDesc));
			ThrowIfFailed(g_SwapChain->ResizeBuffers(g_NumFrames, g_Width,
				g_Height, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

			
			g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

			UpdateRenderTargetViews();
		}
	}

	
	
	//
	// Reads a pre-compiled shader file into a blob!
	//
	HRESULT GfxBackend::ReadShader(LPCWSTR InSrcFile, ID3DBlob** OutBlob)
	{
		if (!InSrcFile || !OutBlob)
			return E_INVALIDARG;

		ID3DBlob* shaderBlob = nullptr;
		*OutBlob = nullptr;
		HRESULT hr = D3DReadFileToBlob(InSrcFile, &shaderBlob);

		if (FAILED(hr))
		{
			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*OutBlob = shaderBlob;

		return hr;
	}

	//
	// Compiles a shader from a file!
	// Function taken from https://docs.microsoft.com/en-us/windows/desktop/direct3d11/how-to--compile-a-shader
	//
	HRESULT GfxBackend::CompileShader(LPCWSTR InSrcFile, LPCSTR InEntryPoint, LPCSTR InProfile, ID3DBlob** OutBlob)
	{
		if (!InSrcFile || !InEntryPoint || !InProfile || !OutBlob)
			return E_INVALIDARG;

		*OutBlob = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif

		const D3D_SHADER_MACRO defines[] =
		{
			"EXAMPLE_DEFINE", "1",
			NULL, NULL
		};

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(InSrcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			InEntryPoint, InProfile,
			flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*OutBlob = shaderBlob;

		return hr;
	}


	void GfxBackend::Flush()
	{
		g_DirectQueue->Flush();
		g_ComputeQueue->Flush();
		g_CopyQueue->Flush();
	}

	//
	//
	//
	u32 GfxBackend::Present(const GfxTexture& InTexture)
	{
		auto commandQueue = GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		//auto commandList = commandQueue->GetCommandList();

		//auto& backBuffer = g_BackBufferTextures[g_CurrentBackBufferIndex];

		/*if (InTexture.IsValid())
		{
			if (InTexture.GetD3D12ResourceDesc().SampleDesc.Count > 1)
			{
				commandList->ResolveSubresource(backBuffer, InTexture);
			}
			else
			{
				commandList->CopyResource(backBuffer, InTexture);
			}
		}*/

		//GfxRenderTarget renderTarget;
		//renderTarget.AttachTexture(AttachmentPoint::Color0, backBuffer);

		//commandList->TransitionBarrier(InTexture, D3D12_RESOURCE_STATE_PRESENT);
		//commandQueue->ExecuteCommandList(commandList);

		//UINT syncInterval = m_VSync ? 1 : 0;
		//UINT presentFlags = m_IsTearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
		g_SwapChain->Present((g_UseVsync) ? 1 : 0, 0);

		g_FrameFenceValues[g_CurrentBackBufferIndex] = commandQueue->Signal();
		g_FrameFenceValues[g_CurrentBackBufferIndex] = Application::GetFrameCount();

		g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();
		commandQueue->WaitForFenceValue(g_FrameFenceValues[g_CurrentBackBufferIndex]);
		GfxBackend::ReleaseStaleDescriptors(g_FrameFenceValues[g_CurrentBackBufferIndex]);

		return g_CurrentBackBufferIndex;
	}

	D3D12_VIEWPORT GfxBackend::GetViewport()
	{
		return g_Viewport;
	}

	D3D12_RECT GfxBackend::GetScissorRect()
	{
		return g_ScissorRect;
	}

	ComPtr<ID3D12Device2> GfxBackend::GetDevice()
	{
		return g_Device;
	}

	ComPtr<IDXGISwapChain3> GfxBackend::GetSwapChain()
	{
		return g_SwapChain;
	}

	std::shared_ptr<GfxQueue> GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		std::shared_ptr<GfxQueue> CommandQueue;
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			CommandQueue = g_DirectQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			CommandQueue = g_ComputeQueue;
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			CommandQueue = g_CopyQueue;
			break;
		default:
			assert(false && "Invalid command queue type.");
		}

		return CommandQueue;
	}

	const GfxRenderTarget& GfxBackend::GetRenderTarget()
	{
		g_RenderTarget.AttachTexture(AttachmentPoint::Color0, g_BackBufferTextures[g_CurrentBackBufferIndex]);	
		return g_RenderTarget;
	}

	DXGI_SAMPLE_DESC GfxBackend::GetMultisampleQualityLevels(DXGI_FORMAT InBackBufferFormat, u32 InNumSamples,
		D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS InFlags)
	{
		DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };

		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
		qualityLevels.Format = InBackBufferFormat;
		qualityLevels.SampleCount = 1;
		qualityLevels.Flags = InFlags;
		qualityLevels.NumQualityLevels = 0;

		bool doesSupportMultisample = SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels,
			sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

		while (qualityLevels.SampleCount <= InNumSamples && doesSupportMultisample && qualityLevels.NumQualityLevels > 0)
		{
			// That works...
			sampleDesc.Count = qualityLevels.SampleCount;
			sampleDesc.Quality = qualityLevels.NumQualityLevels - 1;

			// But can we do better?
			qualityLevels.SampleCount *= 2;
		}

		return sampleDesc;
	}

	GfxDescriptorAllocation GfxBackend::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InDescriptors)
	{
		return g_DescriptorAllocators[InType]->Allocate(InDescriptors);
	}

	void GfxBackend::ReleaseStaleDescriptors(uint64_t finishedFrame)
	{
		for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
		{
			g_DescriptorAllocators[i]->ReleaseStaleDescriptors(finishedFrame);
		}
	}

	u32 GfxBackend::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		return g_Device->GetDescriptorHandleIncrementSize(type);
	}

	u32 GfxBackend::CreateRootSignature(u32 InNumParams, CD3DX12_ROOT_PARAMETER1* InParams, u32 InNumSampler, 
		const CD3DX12_STATIC_SAMPLER_DESC* InSampler,
		D3D12_ROOT_SIGNATURE_FLAGS InFlags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(g_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(InNumParams, InParams, InNumSampler, InSampler, InFlags);

		std::shared_ptr<GfxRootSignature> sig = std::make_shared<GfxRootSignature>();
		sig->SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
		g_RootSignatures.emplace_back(sig);
		return static_cast<u32>(g_RootSignatures.size() - 1);
	}

	std::shared_ptr<GfxRootSignature> GfxBackend::GetRootSignature(u32 InIndex)
	{
		if (g_RootSignatures.size() < InIndex)
			return nullptr;

		return g_RootSignatures[InIndex];
	}

	u32 GfxBackend::CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* InDesc)
	{
		ComPtr<ID3D12PipelineState> state;
		ThrowIfFailed(g_Device->CreateGraphicsPipelineState(InDesc, IID_PPV_ARGS(&state)));
		g_PipelineStates.emplace_back(state);
		return static_cast<u32>(g_PipelineStates.size() - 1);
	}

	ComPtr<ID3D12PipelineState> GfxBackend::GetPipelineState(u32 InIndex)
	{
		if (g_PipelineStates.size() < InIndex)
			return nullptr;

		return g_PipelineStates[InIndex];
	}
}
#endif