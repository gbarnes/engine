#pragma once
#include "inc_common.h"

namespace Dawn
{
	typedef GenericHandle EntityId;

	struct DAWN_API Entity
	{
		EntityId Id;
		std::string Name;
		bool PersistsThroughLoad = false;
	};
}