#pragma once
#ifdef USE_DX12_GFX
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"

namespace Dawn 
{
	class GfxDevice;

	//-----------------------------------------------------------------------------
	// Container for parallel graphics processing
	//-----------------------------------------------------------------------------
	namespace GfxParallel
	{
		//-------------------------------------------------------------------------
		// Public Members
		//-------------------------------------------------------------------------
		const static int ThreadCount = 8;

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		EResult AllocateResources(GfxDevice* GfxDevice);
		void FreeResources();
		ID3D12CommandQueue* ObtainQueue();

		static void AsyncCompute(GfxDevice* InDevice, int InThreadIndex);
	};
}
#endif