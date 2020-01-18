#include "stdafx.h"
#include "Type.h"
#include <map>

namespace Dawn
{
	std::map<std::string, DawnType*>* DawnType::Types = nullptr;
}