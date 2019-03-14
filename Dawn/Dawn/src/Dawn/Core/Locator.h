#pragma once

#include "Container/Map.h"
#include "inc_common.h"
#include <type_traits>

namespace Dawn 
{
	class CEObject;

	class DAWN_API CLocator 
	{

	public:

		template <typename T>
		static T* Get(std::string& InId)
		{
			static_assert(std::is_base_of<CEObject, T>::value, "T must derive from CEObject");
			
			CEObject* object = Instances[InId];
			if (object == nullptr)
				return nullptr;

			return dynamic_cast<T*>(object);
		}
		static void Add(std::string& InId, Dawn::CEObject* InInstance);
		static void Remove(std::string& InId);

	private:

		static Dawn::CMap< std::string, Dawn::CEObject* > Instances;
	};


};
