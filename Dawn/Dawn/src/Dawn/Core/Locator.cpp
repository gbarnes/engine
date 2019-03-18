#include "Locator.h"
#include "EObject.h"

namespace Dawn
{
	Dawn::Map< std::string, Dawn::EObject* > Locator::Instances;

	void Locator::Add(std::string& InId, Dawn::EObject* InInstance)
	{
		InInstance->AddRef();
		Instances.Push(InId, InInstance);
	}

	void Locator::Remove(std::string& InId)
	{
		if (!Instances.Exists(InId))
			return;

		auto instance = Instances.Remove(InId);
		instance->Release();
	}
}