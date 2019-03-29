#pragma once
#include "inc_common.h"

namespace Dawn
{
	class DAWN_API GfxGDI
	{
	public:

		virtual bool Init(const AppSettings& InSettings) = 0;

		virtual void Present() = 0;
		virtual void Shutdown() = 0;
	};
}