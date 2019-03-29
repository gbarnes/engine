#pragma once
#include "inc_common.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

namespace Dawn
{
	typedef glm::mat4 mat4;
	typedef glm::vec4 vec4;
	typedef glm::vec3 vec3;
	typedef glm::vec2 vec2;

	struct DAWN_API VertexPosUV
	{
		vec3 Position;
		vec2 UV0;
		vec2 UV1;
	};
};