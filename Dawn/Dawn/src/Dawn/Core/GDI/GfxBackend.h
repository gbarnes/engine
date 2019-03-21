#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3dcompiler.lib")

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"
#include "inc_common.h"
//#include "GfxQueue.h"

namespace Dawn 
{
	
	class GfxDevice;
	class GfxQueue;
	class GfxDescriptorAllocation;
	class GfxCmdList;

	namespace GfxBackend 
	{
		EResult Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync, bool InIsFullscreen);

		bool IsInitialized();

		void Shutdown();
		void Reset(ComPtr<ID3D12GraphicsCommandList2> InCmdList);
		void ClearRenderTarget(ComPtr<ID3D12GraphicsCommandList2> InCmdList, ComPtr<ID3D12Resource> InRenderTarget, DirectX::XMFLOAT4 InColor);
		void ClearDepthBuffer(ComPtr<ID3D12GraphicsCommandList2> InCmdList, float InDepth);
		void Present(std::shared_ptr<GfxCmdList> InCmdList);
		void Resize(u32 InWidth, u32 InHeight);

		void TransitionResource(ComPtr<ID3D12GraphicsCommandList2> InCmdList, ComPtr<ID3D12Resource> InResource, D3D12_RESOURCE_STATES InPreviousState, D3D12_RESOURCE_STATES InState);

		HRESULT CompileShader(LPCWSTR InSrcFile, LPCSTR InEntryPoint, LPCSTR InProfile, ID3DBlob** OutBlob);
		HRESULT ReadShader(LPCWSTR InSrcFile, ID3DBlob** OutBlob);

		void UpdateBufferResource(ComPtr<ID3D12GraphicsCommandList2> InCommandList, ID3D12Resource** OutDestinationResource, ID3D12Resource** OutIntermediateResource,
			size_t InNumElements, size_t InElementSize, const void* InBufferData, D3D12_RESOURCE_FLAGS Inflags);

		GfxDescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InDescriptors = 1);
		u32 GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);
		void ReleaseStaleDescriptors(uint64_t finishedFrame);

		std::shared_ptr<GfxQueue> GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		GfxDevice* GetDevice();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackbufferDescHandle();
		ComPtr<ID3D12Resource> GetCurrentBackbuffer();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthBufferDescHandle();
		ComPtr<ID3D12Resource> GetDepthBuffer();
	}
};