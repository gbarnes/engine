#pragma once
#include "Layer.h"

namespace Dawn
{
	class Application;

	class GameplayLayer : public Layer
	{
	public:
		GameplayLayer(Shared<Application> InApplication, HWND InWindowHandle);
		~GameplayLayer();

		virtual void Setup();
		virtual void Process(float InDeltaTime);
		virtual void Free();
	};
}