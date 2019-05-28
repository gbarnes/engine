#pragma once
#include "inc_common.h"
#include "Layer.h"

namespace Dawn
{
	class GameplayLayer : public Layer
	{
	public:
		GameplayLayer(HWND InWindowHandle);
		~GameplayLayer();

		virtual void Setup();
		virtual void Process();
		virtual void Free();
	};
}