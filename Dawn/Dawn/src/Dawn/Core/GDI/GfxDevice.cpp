//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "GfxDevice.h"
#include "GfxParallel.h"

namespace Dawn
{
	CGfxDevice::CGfxDevice()
	{
		this->Device = nullptr;
		this->SwapChain = nullptr;
	}

	CGfxDevice::~CGfxDevice(void)
	{

	}

	//
	// Takes care of initializing the directx12 api, swap chain and backbuffers
	//
	EResult CGfxDevice::Initialize(int InWidth, int InHeight, HWND InHWnd, bool InUseVSync, bool InIsFullscreen)
	{
		ComPtr<IDXGIAdapter3> Adapter = GetAdapter(false);
		this->Device = CreateDevice(Adapter).Get();

		this->BackbufferQueue = this->CreateQueue(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT);

		this->SwapChain = CreateSwapChain(InHWnd, this->BackbufferQueue, InWidth, InHeight, g_NumFrames);
		this->BackBufferIndex = this->SwapChain->GetCurrentBackBufferIndex();

		this->RenderTargetViewHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_NumFrames);
		this->RTVDescriptorSize = this->Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		UpdateRenderTargetViews();

		return EResult_OK;
	}

	//
	// Clean-Up your mess
	//
	void CGfxDevice::Shutdown()
	{
		for (int i = 0; i < g_NumFrames; ++i)
		{
			this->BackBufferRenderTarget[i].Reset();
		}

		this->RenderTargetViewHeap.Reset();

		this->BackbufferQueue.Reset();
		this->SwapChain.Reset();
		this->Device.Reset();

		return;
	}

	//
	// Creates the basic D3D12 Device from a given adapter. In case the debug mode 
	// is enable it will log specific statements.
	//
	ComPtr<ID3D12Device> CGfxDevice::CreateDevice(ComPtr<IDXGIAdapter3> InAdapter)
	{
		ComPtr<ID3D12Device> device;

		ThrowIfFailed(D3D12CreateDevice(InAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));


#if defined(_DEBUG)

		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(device.As(&pInfoQueue)))
		{
			pInfoQueue->GetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION);
			pInfoQueue->GetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR);
			pInfoQueue->GetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING);

			D3D12_MESSAGE_SEVERITY Severities[] = {

				D3D12_MESSAGE_SEVERITY_INFO

			};

			D3D12_MESSAGE_ID DenyIds[] = {

				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
#endif

		return device;
	}

	//
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	//
	void CGfxDevice::EnableDebugLayer()
	{
#if defined(_DEBUG)

		ComPtr<ID3D12Debug> debugInterface;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
		debugInterface->EnableDebugLayer();
#endif
	}

	//
	// Checks whether or not tearing is supported on the target hardware.
	//
	bool CGfxDevice::CheckForTearingSupport()
	{
		//
		// Check why we have an older version of the Windows SDK?!
		//
		return false;
	}

	//
	// Creates / updated the render target views depending on the created descriptor handle.
	//
	void CGfxDevice::UpdateRenderTargetViews()
	{
		auto RtvDescriptorSize = this->Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(this->RenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

		for (int i = 0; i < g_NumFrames; ++i)
		{
			ComPtr<CGfxResource> BackBuffer;
			ThrowIfFailed(this->SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffer)));

			Device->CreateRenderTargetView(BackBuffer.Get(), nullptr, RtvHandle);

			this->BackBufferRenderTarget[i] = BackBuffer;

			RtvHandle.Offset(RtvDescriptorSize);
		}
	}

	//
	// Returns a valid adapter that can be passed when creating the d3d12device
	//
	ComPtr<IDXGIAdapter3> CGfxDevice::GetAdapter(bool useWrap)
	{
		ComPtr<IDXGIFactory4> Factory;
		u32 createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&Factory)));

		ComPtr<IDXGIAdapter1> Adapter1;
		ComPtr<IDXGIAdapter3> Adapter3;

		if (useWrap)
		{
			ThrowIfFailed(Factory->EnumWarpAdapter(IID_PPV_ARGS(&Adapter1)));
			ThrowIfFailed(Adapter1.As(&Adapter3));
		}
		else
		{
			u64 maxDedicatedVideoMemory = 0;
			for (u32 i = 0; Factory->EnumAdapters1(i, &Adapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 AdapterDesc1;
				Adapter1->GetDesc1(&AdapterDesc1);

				// Check to see if the adapter can create a D3D12 device without actually 
				// creating it. The adapter with the largest dedicated video memory
				// is favored
				if ((AdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
					SUCCEEDED(D3D12CreateDevice(Adapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
					AdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
				{
					maxDedicatedVideoMemory = AdapterDesc1.DedicatedVideoMemory;
					ThrowIfFailed(Adapter1.As(&Adapter3));
				}
			}
		}

		return Adapter3;
	}

	//
	// Create a new descriptor heap needed in order to allocate resource views such as 
	// RenderTargetView, ShaderResourceView, UnorderedAccessView or ConstantBufferView.
	//
	ComPtr<CGfxHeapDesciptor> CGfxDevice::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE InType, u32 InNumDescriptors)
	{
		ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC Desc = {
			InType,
			InNumDescriptors
		};

		ThrowIfFailed(this->Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&DescriptorHeap)));

		return DescriptorHeap;
	}

	//
	// Create a new command queue from the given list type
	//
	ComPtr<CGfxQueue> CGfxDevice::CreateQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<CGfxQueue> Queue;

		D3D12_COMMAND_QUEUE_DESC Desc = {
			type,
			D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			D3D12_COMMAND_QUEUE_FLAG_NONE,
			0
		};

		ThrowIfFailed(this->Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(&Queue)));

		return Queue;
	}

	//
	// A command allocator is the backing memory used by a command list. The command allocator does not provide any functionality 
	// and can only be accessed indirectly through a command list.  A command allocator can only be used by a single command list at a time 
	// but can be reused after the commands that were recorded into the command list have finished executing on the GPU.
	//
	ComPtr<CGfxCmdAllocator> CGfxDevice::CreateCmdAllocator(D3D12_COMMAND_LIST_TYPE InType)
	{
		ComPtr<CGfxCmdAllocator> CommandAllocator;
		ThrowIfFailed(this->Device->CreateCommandAllocator(InType, IID_PPV_ARGS(&CommandAllocator)));
		return CommandAllocator;
	}

	//
	// A command list is used for recording commands that are executed on the GPU.Unlike
	// previous versions of DirectX, execution of commands recorded into a command list are 
	// always deferred.That is, invoking draw or dispatch commands on a command list are 
	// not executed until the command list is sent to the command queue.
	//
	ComPtr<CGfxCmdList> CGfxDevice::CreateCmdList(ComPtr<CGfxCmdAllocator> InAllocator, D3D12_COMMAND_LIST_TYPE InType)
	{
		ComPtr<CGfxCmdList> List;

		ThrowIfFailed(this->Device->CreateCommandList(0, InType, InAllocator.Get(), nullptr, IID_PPV_ARGS(&List)));

		// Immediate closing of list is necessary so we can use it directly.
		ThrowIfFailed(List->Close());

		return List;
	}

	//
	// The ID3D12Fence is an interface for a GPU / CPU synchronization object. 
	// Fences can be used to perform synchronization on either the CPU or the GPU.
	//
	ComPtr<CGfxFence> CGfxDevice::CreateFence()
	{
		ComPtr<ID3D12Fence> Fence;

		ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));

		return Fence;
	}

	//
	// Creates a new swapchain from a given queue and hwnd.
	//
	ComPtr<IDXGISwapChain3> CGfxDevice::CreateSwapChain(HWND InHwnd, ComPtr<CGfxQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount)
	{
		ComPtr<IDXGISwapChain3> SwapChain3;
		ComPtr<IDXGIFactory4> Factory;
		u32 createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&Factory)));

		DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
		SwapChainDesc.Width = InWidth;
		SwapChainDesc.Height = InHeight;
		SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SwapChainDesc.Stereo = false;
		SwapChainDesc.SampleDesc = { 1, 0 };
		SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		SwapChainDesc.BufferCount = InBufferCount;
		SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		SwapChainDesc.Flags = CheckForTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> SwapChain1;
		ThrowIfFailed(Factory->CreateSwapChainForHwnd(InQueue.Get(), InHwnd, &SwapChainDesc, nullptr, nullptr, &SwapChain1));

		//Switching to a full screen state will be handled manually using a full - screen borderless window.
		//In order to prevent DXGI from switching to a full screen state automatically when pressing the Alt + Enter key 
		//combination on the keyboard
		ThrowIfFailed(Factory->MakeWindowAssociation(InHwnd, DXGI_MWA_NO_ALT_ENTER));
		ThrowIfFailed(SwapChain1.As(&SwapChain3));

		return SwapChain3;
	}

	//
	//
	//
	void CGfxDevice::Present()
	{
		if (!SwapChain)
			return;

		SwapChain->Present((this->bUseVSync) ? 1 : 0, 0);
		this->BackBufferIndex = this->SwapChain->GetCurrentBackBufferIndex();
	}
}
