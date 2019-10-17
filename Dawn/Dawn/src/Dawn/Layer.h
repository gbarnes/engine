#pragma once

#include "inc_common.h"
#include "inc_core.h"

namespace Dawn
{
	class Application;
	class Renderer;

	class DAWN_API Layer
	{
	public:
		Layer(Shared<Dawn::Application> InApplication);
		~Layer();

		virtual void Setup() = 0;
		virtual void Free() = 0;

		virtual void Update(float InDeltaTime)
		{

		}

		virtual void FixedUpdate(float InFixedDeltaTime)
		{

		}

		virtual void Render()
		{

		}

	protected:
		Shared<Application> Application;
	};
}

