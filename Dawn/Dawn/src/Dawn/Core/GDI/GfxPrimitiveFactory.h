#pragma once

#include "inc_gfx.h"

namespace Dawn
{
	class GfxVertexArray;

	//
	// A factory that will create different kinds of 3D mesh primitives
	// as vertex arrays that can be later be used for rendering.
	//
	class GfxPrimitiveFactory
	{
	public:

		static GfxVertexArray* AllocateGrid(GfxGDI* InGDI);
		static GfxVertexArray* AllocateLine(GfxGDI* InGDI);
		//static GfxVertexArray* AllocateQuad();
		//static GfxVertexArray* AllocateSphere();
		//static GfxVertexArray* AllocateCircle();
	};
}