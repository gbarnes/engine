#pragma once
#include <inc_common.h>
#include "../Component.h"

namespace Dawn
{
	struct DAWN_API Transform : public Component<Transform>
	{
		static const u32 Version = 1;
		vec3 Position;
		vec3 Scale;
		quat Rotation;
	};

#define MAKE_TRANSFORM(pos, scale, rot) Transform() {pos, scale,  rot}
}