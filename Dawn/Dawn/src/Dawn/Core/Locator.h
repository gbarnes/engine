#pragma once

#include "Container/Map.h"
#include "inc_common.h"
#include <type_traits>

namespace Dawn 
{
	class EObject;

	class DAWN_API Locator 
	{

	public:

		template <typename T>
		static T* Get(std::string&& InId)
		{
			static_assert(std::is_base_of<EObject, T>::value, "T must derive from CEObject");
			
			EObject* object = Instances[InId];
			if (object == nullptr)
				return nullptr;

			return dynamic_cast<T*>(object);
		}
		static void Add(std::string&& InId, Dawn::EObject* InInstance);
		static void Remove(std::string&& InId);

	private:

		static std::map< std::string, Dawn::EObject* > Instances;
	};


};
