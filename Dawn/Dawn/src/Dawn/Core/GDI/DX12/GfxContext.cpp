#include "stdafx.h"
#ifdef USE_DX12_GFX
#include "GfxContext.h"
#include "inc_gfx.h"

namespace Dawn 
{
	namespace GfxContext 
	{
		void EnableDebugLayer()
		{
#if defined(_DEBUG)

			ComPtr<ID3D12Debug> debugInterface;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
			debugInterface->EnableDebugLayer();
#endif
		}

		//
		// Determines the current adapter that is being used.
		//
		ComPtr<IDXGIAdapter3> GfxContext::GetAdapter(bool InUseWrap)
		{
			ComPtr<IDXGIFactory4> Factory;
			u32 createFactoryFlags = 0;
#if defined(_DEBUG)
			createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&Factory)));

			ComPtr<IDXGIAdapter1> Adapter1;
			ComPtr<IDXGIAdapter3> Adapter3;

			if (InUseWrap)
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
						maxDedicatedVideoMemory = (u64)AdapterDesc1.DedicatedVideoMemory;
						ThrowIfFailed(Adapter1.As(&Adapter3));
					}
				}
			}

			return Adapter3;
		}


		ComPtr<ID3D12Device2> GfxContext::CreateDevice(ComPtr<IDXGIAdapter3> InAdapter)
		{
			EnableDebugLayer();

			ComPtr<ID3D12Device2> device;
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


		bool GfxContext::CheckForTearingSupport()
		{
			return false;
		}

		bool GfxContext::SupportsMath()
		{
			// Check for DirectX Math library support.
			if (!DirectX::XMVerifyCPUSupport())
			{
				MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
				return false;
			}

			return true;
		}
		
		ComPtr<IDXGISwapChain3> GfxContext::CreateSwapChain(HWND InHwnd, ComPtr<ID3D12CommandQueue> InQueue, u32 InWidth, u32 InHeight, u32 InBufferCount)
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



	}
}
#endif