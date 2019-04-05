#pragma once
#include "inc_common.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

namespace Dawn
{
	struct DAWN_API VertexPos
	{
		vec3 Position;
	};

	struct DAWN_API VertexPosUV1
	{
		vec3 Position;
		vec2 UV0;
		vec2 UV1;
	};

	struct DAWN_API VertexPosUV
	{
		vec3 Position;
		vec2 UV0;
		vec2 UV1;
	};


	struct GfxWrapDesc
	{
		i32 WrapS;
		i32 WrapT;
	};

	struct GfxFilterDesc
	{
		i32 Min;
		i32 Mag;
	};
};