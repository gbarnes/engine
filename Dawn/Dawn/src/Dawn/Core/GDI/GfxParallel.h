#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			 //

//-----------------------------------------------------------------------------
// Inclusions
//-----------------------------------------------------------------------------
#include "inc_gfx.h"

namespace Dawn 
{
	class CGfxDevice;

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
		EResult AllocateResources(CGfxDevice* GfxDevice);
		void FreeResources();
		CGfxInternalQueue* ObtainQueue();

		static void AsyncCompute(CGfxDevice* InDevice, int InThreadIndex);
	};
}