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
		virtual void Process() = 0;
		virtual void Free() = 0;

	protected:
		Shared<Application> Application;
	};
}

