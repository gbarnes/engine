#pragma once

#include "inc_common.h"
#include "Layer.h"

#include "EntitySystem/PhysicsWorld.h"
#include "EntitySystem/World.h"

namespace Dawn
{
	class WorldSimulateLayer : public Layer
	{
	public:
		WorldSimulateLayer(Shared<Dawn::Application> InApplication);
		~WorldSimulateLayer();

		virtual void Setup();
		virtual void Free();
		virtual void FixedUpdate(float InFixedDeltaTime) override;
	};
}