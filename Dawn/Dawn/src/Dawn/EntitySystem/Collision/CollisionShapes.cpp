#include "stdafx.h"
#include "CollisionShapes.h"
#include "ResourceSystem/Resources.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"

namespace Dawn
{
	void BoxShape::InitFromLoad(World* InWorld, void* Component)
	{
		auto* shape = static_cast<BoxShape*>(Component);
		shape->WorldRef = InWorld;
	}

	void CollisionUtils::CalculateBoundingBox(u32 InAmount, BoxShape** InShapes, Model** InModels)
	{
		auto rs = g_Application->GetResourceSystem();

		for (u32 i = 0; i < InAmount; ++i)
		{
			auto* shape = InShapes[i];
			auto* model = InModels[i];
			
			for (auto mesh : model->Meshes)
			{
				Mesh* meshObj = rs->FindMesh(mesh);
				
			}
		}
	}
}