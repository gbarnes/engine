#pragma once

#include "inc_common.h"
#include "inc_core.h"

namespace Dawn
{
	class DAWN_API Layer : public EObject
	{
	public:
		Layer();
		~Layer();

		virtual void Setup() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Free() = 0;
	};
}

