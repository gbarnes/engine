#pragma once
#include "inc_common.h"
#include "../GfxShader.h"

namespace Dawn
{
	class GLVertexShader : public GfxFragShader
	{
	private:
		u32 ResourceId;
	};

	class GLFragShader : public GfxFragShader
	{
	private:
		u32 ResourceId;
	};
}