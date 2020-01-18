#pragma once
#include "Core/Type.h"
#include "ResourceSystem/Resources.h"
#include "../Component.h"

namespace Dawn
{
	class ResourceSystem;

	struct DAWN_API ModelView : public Component<ModelView>
	{
		REGISTER_TYPE(ModelView)
		static const u32 Version = 1;

		ModelView()
		{}

		FileId ModelId;
		ResourceId ResourceId;

		Model* GetModel(ResourceSystem* InResourceSystem);

		static void InitFromLoad(World* InWorld, void* Component);
	};


	extern ModelView* CreateModelView(World* InWorld, const Entity& InEntity, const std::string& InFile);
}