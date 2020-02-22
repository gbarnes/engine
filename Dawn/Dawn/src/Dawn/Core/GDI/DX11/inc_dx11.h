#pragma once
#include <d3d11.h>
#include "D3DX11.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

FORCEINLINE void SafeRelease(IUnknown* Interface) 
{
	if (Interface != nullptr) 
	{
		Interface->Release();
		Interface = nullptr;
	}
}