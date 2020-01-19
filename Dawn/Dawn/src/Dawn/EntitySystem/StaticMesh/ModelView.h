#pragma once
#include "Core/Type.h"
#include "ResourceSystem/Resources.h"
#include "../Component.h"
#include "Rendering/inc_renderer_common.h"

namespace Dawn
{
	class ResourceSystem;

	struct DAWN_API ModelView : public Component<ModelView>
	{
		REGISTER_TYPE(ModelView)
		static const u32 Version = 1;

		ModelView()
		{}

		RenderLayer Layer = RenderLayer::StaticGeom;
		FileId ModelId;
		ResourceId ResourceId;

		Model* GetModel(ResourceSystem* InResourceSystem);

		static void InitFromLoad(World* InWorld, void* Component);
	};


	extern ModelView* CreateModelView(World* InWorld, const Entity& InEntity, const std::string& InFile);
}