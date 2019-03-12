#pragma once

#include "Container/Map.h"
#include "inc_common.h"

namespace Dawn 
{
	class CEObject;

	class DAWN_API CLocator 
	{

	public:

		static Dawn::CEObject* Get(std::string& InId);
		static void Add(std::string& InId, Dawn::CEObject* InInstance);
		static void Remove(std::string& InId);

	private:

		static Dawn::CMap< std::string, Dawn::CEObject* > Instances;
	};


};
