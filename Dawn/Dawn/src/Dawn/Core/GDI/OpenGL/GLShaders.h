#pragma once
#include "inc_common.h"
#include "../GfxShader.h"

namespace Dawn
{
	class OpenGLVertexShader : public GfxVertexShader
	{
	private:
		u32 InternalId;
	};

	class OpenGLFragShader : public GfxFragShader
	{
	private:
		u32 InternalId;
	};
}