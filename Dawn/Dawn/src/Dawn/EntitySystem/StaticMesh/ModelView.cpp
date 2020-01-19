#include "stdafx.h"
#include "ModelView.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"
#include "EntitySystem/World.h"

namespace Dawn
{
	MAKE_TYPE_BEGIN(ModelView)
		ADD_MEMBER(ModelView, Layer, I32Type, true)
		ADD_MEMBER(ModelView, ModelId, U64Type, true)
	MAKE_TYPE_END()

	void ModelView::InitFromLoad(World* InWorld, void* Component)
	{
		ModelView* view = static_cast<ModelView*>(Component);
		view->WorldRef = InWorld;
		view->ResourceId = g_Application->GetResourceSystem()->LoadFile(view->ModelId);
	}

	Model* ModelView::GetModel(ResourceSystem* InResourceSystem)
	{
		return InResourceSystem->FindModel(ResourceId);
	}

	ModelView* CreateModelView(World* InWorld, const Entity& InEntity, const std::string& InFile)
	{
		auto* view = InWorld->MakeComponent<ModelView>();
		auto rs = g_Application->GetResourceSystem();

		view->ModelId = rs->GetFileId(InFile);
		view->ResourceId = rs->LoadFile(InFile);
		return view;
	}
}