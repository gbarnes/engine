//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "GfxDevice.h"
#include "GfxParallel.h"

///////////////////////////////////////////////////////////////////////////////
// CGfxDevice Member Definitions
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Creates a new instance of the device
//-----------------------------------------------------------------------------
CGfxDevice::CGfxDevice()
{
	this->Device = nullptr;
	this->SwapChain = nullptr;
	this->PipelineState = nullptr;
	this->Fence = nullptr;
	this->BackBufferRenderTarget[0] = nullptr;
	this->BackBufferRenderTarget[1] = nullptr;
}

CGfxDevice::~CGfxDevice(void)
{

}

//-----------------------------------------------------------------------------
// Increses the reference count by one. Call this whenever you reference the 
// object!
//-----------------------------------------------------------------------------
EResult CGfxDevice::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync, bool InIsFullscreen)
{
	HRESULT Result;

	/*********************************************************/
	D3D_FEATURE_LEVEL FeatureLevel;
	IDXGIFactory4* Factory;
	unsigned int Numerator = 0, Denominator = 0;
	/*********************************************************/

	// Store the vsync setting.
	this->bUseVSync = InUseVSync;
	FeatureLevel = D3D_FEATURE_LEVEL_12_1;

	// Start creating the DirectX Device;
	Result = D3D12CreateDevice(NULL, FeatureLevel, __uuidof(ID3D12Device), (void**)&this->Device);
	if (FAILED(Result))
	{
		MessageBox(InHWnd, L"Could not create a DirectX 12.1 device.  The default video card does not support DirectX 12.1.", L"DirectX Device Failure", MB_OK);
		return EResult_OUT_OF_MEMORY;
	}

	// Now create the command queue
	if (GfxParallel::AllocateResources(this) == EResult_ERROR)
	{
		return EResult_ERROR;
	}

	// Create a DirectX graphics interface factory.
	Result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&Factory);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Determine the adapter info (which graphic card do we use?).
	if (DetermineAdapterInfo(Factory, InWidth, InHeight, &Numerator, &Denominator) == EResult_ERROR)
	{
		return EResult_ERROR;
	}

	// Initialize the swap chain 
	if (CreateSwapChain(InWidth, InHeight, InHWnd, InIsFullscreen, &Numerator, &Denominator, Factory) == EResult_ERROR)
	{
		return EResult_ERROR;
	}


	// Release the factory now that the swap chain has been created.
	Factory->Release();
	Factory = 0;
	
	// creates the back buffer render targets for the swap chain
	// this might be outsourced to specific render target classes
	if (CreateBackbufferRT() == EResult_ERROR)
	{
		return EResult_ERROR;
	}

	// Finally get the initial index to which buffer is the current back buffer.
	BufferIndex = SwapChain->GetCurrentBackBufferIndex();

	// Create a fence for GPU synchronization.
	Result = Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&Fence);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Create an event object for the fence.
	FenceEvent = CreateEventEx(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (FenceEvent == NULL)
	{
		return EResult_ERROR;
	}

	// Initialize the starting fence value. 
	FenceValue = 1;
	
	return EResult_OK;
}

//-----------------------------------------------------------------------------
// Clears memory. 
//-----------------------------------------------------------------------------
void CGfxDevice::Shutdown()
{
	int Error = 0;

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (SwapChain)
	{
		SwapChain->SetFullscreenState(false, NULL);
	}

	Error = CloseHandle(FenceEvent);
	if (Error == 0)
	{
	}

	// Release the fence.
	if (Fence)
	{
		Fence->Release();
		Fence = 0;
	}


	// Release the empty pipe line state.
	if (PipelineState)
	{
		PipelineState->Release();
		PipelineState = 0;
	}


	// Release the back buffer render target views.
	if (BackBufferRenderTarget[0])
	{
		BackBufferRenderTarget[0]->Release();
		BackBufferRenderTarget[0] = 0;
	}
	if (BackBufferRenderTarget[1])
	{
		BackBufferRenderTarget[1]->Release();
		BackBufferRenderTarget[1] = 0;
	}

	// Release the render target view heap.
	if (RenderTargetViewHeap)
	{
		RenderTargetViewHeap->Release();
		RenderTargetViewHeap = 0;
	}

	// Release the swap chain.
	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = 0;
	}

	//free resources
	GfxParallel::FreeResources();

	// Release the device.
	if (Device)
	{
		Device->Release();
		Device = 0;
	}

	return;
}

//-----------------------------------------------------------------------------
// Creates a new swap chain object.
//-----------------------------------------------------------------------------
EResult CGfxDevice::CreateSwapChain(int InWidth, int InHeight, HWND InHWnd, bool InIsFullscreen, unsigned int* InNumerator, unsigned int* InDenominator, IDXGIFactory4* InFactory)
{
	/*********************************************************/
	HRESULT Result;
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	IDXGISwapChain* LSwapChain = nullptr;
	/*********************************************************/

	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// Set the swap chain to use double buffering.
	SwapChainDesc.BufferCount = 2;

	// Set the height and width of the back buffers in the swap chain.
	SwapChainDesc.BufferDesc.Height = InHeight;
	SwapChainDesc.BufferDesc.Width = InWidth;

	// Set a regular 32-bit surface for the back buffers.
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the usage of the back buffers to be render target outputs.
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the swap effect to discard the previous buffer contents after swapping.
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Set the handle for the window to render to.
	SwapChainDesc.OutputWindow = InHWnd;

	// Set to full screen or windowed mode.
	SwapChainDesc.Windowed = !InIsFullscreen;

	// Set the refresh rate of the back buffer.
	if (bUseVSync)
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = *InNumerator;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = *InDenominator;
	}
	else
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Turn multisampling off.
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	// Set the scan line ordering and scaling to unspecified.
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Don't set the advanced flags.
	SwapChainDesc.Flags = 0;

	// Finally create the swap chain using the swap chain description.	
	Result = InFactory->CreateSwapChain(GfxParallel::ObtainQueue(), &SwapChainDesc, &LSwapChain);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	// This will allow us to use the newer functionality such as getting the current back buffer index.
	Result = LSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&this->SwapChain);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	LSwapChain = 0;


	return EResult_OK;
}

//-----------------------------------------------------------------------------
// Increses the reference count by one. Call this whenever you reference the 
// object!
//-----------------------------------------------------------------------------
EResult CGfxDevice::CreateBackbufferRT()
{
	/*********************************************************/
	HRESULT Result;
	D3D12_DESCRIPTOR_HEAP_DESC RenderTargetViewHeapDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetViewHandle;
	int RenderTargetViewDescriptorSize;
	/*********************************************************/

	// Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&RenderTargetViewHeapDesc, sizeof(RenderTargetViewHeapDesc));

	// Set the number of descriptors to two for our two back buffers.  Also set the heap tyupe to render target views.
	RenderTargetViewHeapDesc.NumDescriptors = 2;
	RenderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RenderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	Result = Device->CreateDescriptorHeap(&RenderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&RenderTargetViewHeap);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	RenderTargetViewHandle = RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

	// Get the size of the memory location for the render target view descriptors.
	RenderTargetViewDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get a pointer to the first back buffer from the swap chain.
	Result = SwapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&BackBufferRenderTarget[0]);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Create a render target view for the first back buffer.
	Device->CreateRenderTargetView(BackBufferRenderTarget[0], NULL, RenderTargetViewHandle);

	// Increment the view handle to the next descriptor location in the render target view heap.
	RenderTargetViewHandle.ptr += RenderTargetViewDescriptorSize;

	// Get a pointer to the second back buffer from the swap chain.
	Result = SwapChain->GetBuffer(1, __uuidof(ID3D12Resource), (void**)&BackBufferRenderTarget[1]);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Create a render target view for the second back buffer.
	Device->CreateRenderTargetView(BackBufferRenderTarget[1], NULL, RenderTargetViewHandle);

	return EResult_OK;
}

//-----------------------------------------------------------------------------
// Increses the reference count by one. Call this whenever you reference the 
// object!
//-----------------------------------------------------------------------------
EResult CGfxDevice::DetermineAdapterInfo(IDXGIFactory4* InFactory, int InWidth, int InHeight, unsigned int* InNumerator, unsigned int* InDenominator)
{
	/*********************************************************/
	HRESULT Result;
	IDXGIAdapter* Adapter;
	IDXGIOutput* AdapterOutput;
	DXGI_MODE_DESC* DisplayModeList = nullptr;
	DXGI_ADAPTER_DESC AdapterDesc;

	int Error;
	unsigned int NumModes = 0, i = 0, Numerator = 0, Denominator = 0;
	size_t StringLength;
	/*********************************************************/

	// Use the factory to create an adapter for the primary graphics interface (video card).
	Result = InFactory->EnumAdapters(0, &Adapter);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Enumerate the primary adapter output (monitor).
	Result = Adapter->EnumOutputs(0, &AdapterOutput);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}


	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	Result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, NULL);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	DisplayModeList = new DXGI_MODE_DESC[NumModes];


	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	Result = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &NumModes, DisplayModeList);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Now go through all the display modes and find the one that matches the screen height and width.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i< NumModes; i++)
	{
		if (DisplayModeList[i].Height == (unsigned int)InHeight)
		{
			if (DisplayModeList[i].Width == (unsigned int)InWidth)
			{
				Numerator = DisplayModeList[i].RefreshRate.Numerator;
				Denominator = DisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	(*InNumerator) = Numerator;
	(*InDenominator) = Denominator;

	// Get the adapter (video card) description.
	Result = Adapter->GetDesc(&AdapterDesc);
	if (FAILED(Result))
	{
		return EResult_ERROR;
	}

	// Store the dedicated video card memory in megabytes.
	int VideoCardMemory = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// &StringLength, &VideoCardDescription[0], 128, AdapterDesc.Description, 128);
	// Convert the name of the video card to a character array and store it.
	Error = wcstombs_s(&StringLength, VideoCardDescription, 128, AdapterDesc.Description, 128);
	if (Error != 0)
	{
		return EResult_ERROR;
	}

	// Release the display mode list.
	delete[] DisplayModeList;
	DisplayModeList = 0;

	// Release the adapter output.
	AdapterOutput->Release();
	AdapterOutput = 0;

	// Release the adapter.
	Adapter->Release();
	Adapter = 0;

	return EResult_OK;
}

void CGfxDevice::Present()
{
	if (!SwapChain)
		return;

	SwapChain->Present((this->bUseVSync) ? 1 : 0, 0);
}

CGfxResource* CGfxDevice::GetCurrentBackbuffer()
{
	return this->BackBufferRenderTarget[this->BufferIndex];
}

void CGfxDevice::WaitForPreviousFrame()
{
	// Signal and increment the fence value.
	unsigned long long FenceToWaitFor = FenceValue;
	HRESULT Result = GfxParallel::ObtainQueue()->Signal(Fence, FenceToWaitFor);
	if (FAILED(Result))
	{
		return;
	}

	FenceValue++;

	// Wait until the GPU is done rendering.
	if (Fence->GetCompletedValue() < FenceToWaitFor)
	{
		Result = Fence->SetEventOnCompletion(FenceToWaitFor, FenceEvent);
		if (FAILED(Result))
		{
			return;
		}

		WaitForSingleObject(FenceEvent, INFINITE);
	}
}