#pragma once

#include "Layer.h"


namespace Dawn
{
	class Application;
	class WorldSimulateLayer : public Layer
	{
	public:
		WorldSimulateLayer(Shared<Application> InApplication);
		~WorldSimulateLayer();

		virtual void Setup();
		virtual void Free();
		virtual void FixedUpdate(float InFixedDeltaTime) override;
	};
}