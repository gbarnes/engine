#pragma once

#include "inc_gfx.h"

namespace Dawn
{
	class GfxVertexArrayObject;

	//
	// A factory that will create different kinds of 3D mesh primitives
	// as vertex arrays that can be later be used for rendering.
	//
	class DAWN_API GfxPrimitiveFactory
	{
	public:

		static GfxVertexArrayObject* AllocateLine(GfxGDI* InGDI);
		static GfxVertexArrayObject* AllocateQuad(GfxGDI* InGDI, const vec2& UVCoords, float InSize = 0.5f);
		static GfxVertexArrayObject* AllocateCube(GfxGDI* InGDI);
		//static GfxVertexArray* AllocateSphere();
		//static GfxVertexArray* AllocateCircle();
	};
}