#pragma once

#include "inc_common.h"
#include "inc_core.h"

namespace Dawn
{
	class Application;

	class DAWN_API Layer
	{
	public:
		Layer(Shared<Dawn::Application> InApplication);
		~Layer();

		virtual void Setup() = 0;
		virtual void Process(float InDeltaTime) = 0;
		virtual void Free() = 0;

		// will both be removed later on again
		// since layers should only be for processing
		// if we keep layers at all...

		virtual void Update(float InDeltaTime)
		{

		}

		virtual void Render()
		{

		}

	protected:
		Shared<Application> Application;
	};
}

