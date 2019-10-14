#pragma once
#include "inc_common.h"
#include "Layer.h"

namespace Dawn
{
	class GameplayLayer : public Layer
	{
	public:
		GameplayLayer(const Shared<Dawn::Application>& InApplication, HWND InWindowHandle);
		~GameplayLayer();

		virtual void Setup();
		virtual void Process(float InDeltaTime);
		virtual void Free();
	};
}