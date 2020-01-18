#pragma once

#include "Core/Type.h"
#include "EntitySystem/Component.h"

namespace Dawn
{
	class Model;

	struct DAWN_API BoxShape : public Component<BoxShape>
	{
		friend class CollisionUtils;
		REGISTER_TYPE(BoxShape)
		static const u32 Version = 1;

		BoxShape()
		{}

		AABBox Bounds;

		static void InitFromLoad(World* InWorld, void* Component);
	};


	class CollisionUtils
	{
	public:
		static void CalculateBoundingBox(u32 InAmount, BoxShape** InShapes, Model** InModels);
	};
}