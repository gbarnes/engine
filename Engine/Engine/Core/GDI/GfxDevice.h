#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "../../inc_common.h"
#include <Windows.h>
#include <wrl/client.h>
#include "inc_gfx.h"


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// The GrahpicsDevice is used to setup DirectX12 and provide usage of it!
//-----------------------------------------------------------------------------
class CGfxDevice
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	CGfxDevice::CGfxDevice();
	CGfxDevice::~CGfxDevice(void);

	//-------------------------------------------------------------------------
	// Public Members
	//-------------------------------------------------------------------------
	EResult CGfxDevice::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync = false, bool InIsFullscreen = false);
	void CGfxDevice::Present();
	void CGfxDevice::Shutdown();
	CGfxResource* CGfxDevice::GetCurrentBackbuffer();
	void WaitForPreviousFrame();

	ID3D12PipelineState* GetPipelineState() 
	{
		return this->PipelineState;
	}

	ID3D12DescriptorHeap* CGfxDevice::GetBackbufferHeap()
	{
		return this->RenderTargetViewHeap;
	}

	unsigned int CGfxDevice::GetBufferIndex()
	{
		return this->BufferIndex;
	}

	//-------------------------------------------------------------------------
	// Operator
	//-------------------------------------------------------------------------
	operator ID3D12Device*() {
		return this->Device;
	}

private:
	ID3D12Device* Device;
	IDXGISwapChain3* SwapChain;
	ID3D12PipelineState* PipelineState;
	ID3D12Fence* Fence;
	ID3D12DescriptorHeap* RenderTargetViewHeap;
	ID3D12Resource* BackBufferRenderTarget[2];
	char VideoCardDescription[128];
	HANDLE FenceEvent;
	unsigned long long FenceValue;
	bool bUseVSync;
	bool bIsFullscreen;
	unsigned int BufferIndex;

	//-------------------------------------------------------------------------
	// Private Members
	//-------------------------------------------------------------------------
	EResult CGfxDevice::CreateSwapChain(int InWidth, int InHeight, HWND InHWnd, bool InIsFullscreen, unsigned int* InNumerator, unsigned int* InDenominator, IDXGIFactory4* InFactory);
	EResult CGfxDevice::CreateBackbufferRT();
	EResult CGfxDevice::DetermineAdapterInfo(IDXGIFactory4* InFactory, int InWidth, int InHeight, unsigned int* InNumerator, unsigned int* InDenominator);
};
