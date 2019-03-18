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

	namespace GfxBackend 
	{
		EResult Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync, bool InIsFullscreen);

		bool IsInitialized();

		void Shutdown();
		void Reset(ComPtr<CGfxCmdList> InCmdList);
		void ClearRenderTarget(ComPtr<CGfxCmdList> InCmdList, ComPtr<CGfxResource> InRenderTarget, DirectX::XMFLOAT4 InColor);
		void ClearDepthBuffer(ComPtr<CGfxCmdList> InCmdList, float InDepth);
		void Present(ComPtr<CGfxCmdList> InCmdList);
		void Resize(u32 InWidth, u32 InHeight);

		void TransitionResource(ComPtr<CGfxCmdList> InCmdList, ComPtr<CGfxResource> InResource, D3D12_RESOURCE_STATES InPreviousState, D3D12_RESOURCE_STATES InState);

		HRESULT CompileShader(LPCWSTR InSrcFile, LPCSTR InEntryPoint, LPCSTR InProfile, ID3DBlob** OutBlob);
		HRESULT ReadShader(LPCWSTR InSrcFile, ID3DBlob** OutBlob);

		void UpdateBufferResource(ComPtr<CGfxCmdList> InCommandList, CGfxResource** OutDestinationResource, CGfxResource** OutIntermediateResource,
			size_t InNumElements, size_t InElementSize, const void* InBufferData, D3D12_RESOURCE_FLAGS Inflags);

		std::shared_ptr<GfxQueue> GetQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
		GfxDevice* GetDevice();

		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackbufferDescHandle();
		ComPtr<CGfxResource> GetCurrentBackbuffer();

		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthBufferDescHandle();
		ComPtr<CGfxResource> GetDepthBuffer();
	}
};