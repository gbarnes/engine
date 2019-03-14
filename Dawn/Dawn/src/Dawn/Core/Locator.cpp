#include "Locator.h"
#include "EObject.h"

namespace Dawn
{
	Dawn::CMap< std::string, Dawn::CEObject* > CLocator::Instances;

	void CLocator::Add(std::string& InId, Dawn::CEObject* InInstance)
	{
		InInstance->AddRef();
		Instances.Push(InId, InInstance);
	}

	void CLocator::Remove(std::string& InId)
	{
		if (!Instances.Exists(InId))
			return;

		auto instance = Instances.Remove(InId);
		instance->Release();
	}
}