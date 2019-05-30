#pragma once
//---------------------------------------------------------------------------//
//        Copyright 2016  Immersive Pixels. All Rights Reserved.			//


#include "inc_gfx_types.h"
#include "GfxGDI.h"

//#ifdef USE_OPENGL_GFX
#include "OpenGL/GLGraphicsDevice.h"
//#endif

namespace Dawn
{
	static std::unique_ptr<GfxGDI> CreateGDI()
	{
//#ifdef USE_OPENGL_GFX
		return std::make_unique<GLGraphicsDevice>();
//#endif
	}
}
