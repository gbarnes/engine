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

		static GfxVertexArray* AllocateGrid();
		static GfxVertexArray* AllocateLine();
		//static GfxVertexArray* AllocateQuad();
		//static GfxVertexArray* AllocateSphere();
		//static GfxVertexArray* AllocateCircle();
	};
}