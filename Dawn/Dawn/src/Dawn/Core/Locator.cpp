#include "Locator.h"
#include "EObject.h"

namespace Dawn
{
	std::map< std::string, Dawn::EObject* > Locator::Instances;

	void Locator::Add(std::string& InId, Dawn::EObject* InInstance)
	{
		InInstance->AddRef();
		Instances.emplace(InId, InInstance);
	}

	void Locator::Remove(std::string& InId)
	{
		auto it = Instances.find(InId);
		if (it == Instances.end())
			return;

		auto instance = it->second;
		Instances.erase(InId);
		instance->Release();
	}
}