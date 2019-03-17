#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"
#include "GfxDevice.h"
#include "GfxParallel.h"
#include "GfxBackend.h"
#include "imgui.h"
#include "GfxQueue.h"
#include "Vendor/ImGui/imgui_impl_win32.h"
#include "Vendor/ImGui/imgui_impl_dx12.h"

namespace Dawn
{
	CGfxDevice g_Device;

	ComPtr<CGfxCmdList> g_CommandList;
	u64 g_FrameFenceValues[g_NumFrames] = {};
	u64 g_FenceValue = 0;
	bool g_Initialized = false;

	D3D12_RESOURCE_STATES g_LastResourceTransitionState = D3D12_RESOURCE_STATE_PRESENT;

	inline HANDLE CreateEventHandle()
	{
		HANDLE FenceEvent;

		FenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(FenceEvent && "Failed to create fence event.");

		return FenceEvent;
	}

	bool GfxBackend::IsInitialized()
	{
		return g_Initialized;
	}

	EResult GfxBackend::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false)
	{
		// Check for DirectX Math library support.
		if (!DirectX::XMVerifyCPUSupport())
		{
			MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
			return EResult_ERROR;
		}

		if (g_Device.Initialize(InWidth, InHeight, InHWnd, InUseVSync, InIsFullscreen) != EResult_OK)
			return EResult_ERROR;

		g_Device.GetQueue()->Flush();
		g_Device.GetQueue(D3D12_COMMAND_LIST_TYPE_COPY)->Flush();
		g_Device.GetQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->Flush();
		g_Device.CreateDepthBuffer(InWidth, InHeight);

		g_Initialized = true;

		return EResult_OK;
	}

	//
	//
	//
	void GfxBackend::Shutdown()
	{
		g_Device.Shutdown();
	}

	void GfxBackend::Resize(u32 InWidth, u32 InHeight)
	{
		//if (g_Device.Width != width || g_ClientHeight != height)
		//{
		if (!IsInitialized())
			return;

		g_Device.Resize(InWidth, InHeight);

		//}
	}

	void GfxBackend::ClearRenderTarget(ComPtr<CGfxCmdList> InCmdList, ComPtr<CGfxResource> InRenderTarget, DirectX::XMFLOAT4 InColor)
	{
		FLOAT clearColor[] = { InColor.x, InColor.y, InColor.z, InColor.w };

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv = GfxBackend::GetCurrentBackbufferDescHandle();
		InCmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	}

	void GfxBackend::ClearDepthBuffer(ComPtr<CGfxCmdList> InCmdList, float InDepth)
	{
		auto Handle = GfxBackend::GetDepthBufferDescHandle();
		InCmdList->ClearDepthStencilView(Handle, D3D12_CLEAR_FLAG_DEPTH, InDepth, 0, 0, nullptr);
	}

	//
	// The UpdateBufferResource method is used to create a ID3D12Resource 
	// that is large enough to store the buffer data passed to the function 
	// and to create an intermediate buffer that is used to copy the CPU buffer data to the GPU.
	//
	void GfxBackend::UpdateBufferResource(ComPtr<CGfxCmdList> InCommandList, CGfxResource** OutDestinationResource, CGfxResource** OutIntermediateResource,
		size_t InNumElements, size_t InElementSize, const void* InBufferData, D3D12_RESOURCE_FLAGS InFlags)
	{
		size_t BufferSize = InNumElements * InElementSize;

		ComPtr<ID3D12Device> Device = g_Device.GetD3D12Device();

		ThrowIfFailed(Device->CreateCommittedResource
		(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(BufferSize, InFlags),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(OutDestinationResource)
		));

		if (InBufferData)
		{
			ThrowIfFailed(Device->CreateCommittedResource
			(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(BufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(OutIntermediateResource)
			));

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = InBufferData;
			subresourceData.RowPitch = BufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(InCommandList.Get(), *OutDestinationResource, *OutIntermediateResource, 0, 0, 1, &subresourceData);
		}
	}

	//
	// Issues a switch state command for a speicfic resource to a passed command list 
	//
	void GfxBackend::TransitionResource(ComPtr<CGfxCmdList> InCmdList, ComPtr<CGfxResource> InResource, D3D12_RESOURCE_STATES InPreviousState, D3D12_RESOURCE_STATES InState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
		(
			InResource.Get(),
			InPreviousState,
			InState,
			D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		);

		InCmdList->ResourceBarrier(1, &barrier);
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
		//flags |= D3DCOMPILE_DEBUG;
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

	//
	//
	//
	void GfxBackend::Reset(ComPtr<CGfxCmdList> InCmdList)
	{
		TransitionResource(InCmdList, GfxBackend::GetCurrentBackbuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		ClearRenderTarget(InCmdList, GfxBackend::GetCurrentBackbuffer(), DirectX::XMFLOAT4({ 0.4f, 0.6f, 0.9f, 1.0f }));
	}

	//
	//
	//
	void GfxBackend::Present(ComPtr<CGfxCmdList> InCmdList)
	{
		auto CommandQueue = g_Device.GetQueue();

		TransitionResource(InCmdList, GfxBackend::GetCurrentBackbuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		u32 BackBufferIndex = g_Device.GetCurrentBufferIndex();
		g_FrameFenceValues[BackBufferIndex] = CommandQueue->ExecuteCommandList(InCmdList);
		BackBufferIndex = g_Device.Present();
		CommandQueue->WaitForFenceValue(g_FrameFenceValues[BackBufferIndex]);
	}

	CGfxDevice* GfxBackend::GetDevice()
	{
		return &g_Device;
	}

	std::shared_ptr<CGfxQueue> GfxBackend::GetQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		return g_Device.GetQueue(type);
	}

	ComPtr<CGfxResource> GfxBackend::GetCurrentBackbuffer()
	{
		return g_Device.GetBackbuffer();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE GfxBackend::GetCurrentBackbufferDescHandle()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
		(
			g_Device.GetRTVHeap()->GetCPUDescriptorHandleForHeapStart(),
			g_Device.GetCurrentBufferIndex(),
			g_Device.GetRTVDescriptorSize()
		);

		return rtv;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GfxBackend::GetDepthBufferDescHandle()
	{
		return g_Device.GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	}

	ComPtr<CGfxResource> GfxBackend::GetDepthBuffer()
	{
		return g_Device.GetDepthBuffer();
	}

}