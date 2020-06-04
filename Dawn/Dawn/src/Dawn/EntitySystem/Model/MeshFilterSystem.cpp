#include "stdafx.h"
#include "MeshFilterSystem.h"
#include "MeshFilter.h"
#include "EntitySystem/Transform/Transform.h"
#include "EntitySystem/World.h"
#include "ResourceSystem/ResourceSystem.h"
#include "Application.h"
#include "Rendering/DeferredRenderer.h"
#include "EntitySystem/Camera/Camera.h"
#include <Brofiler.h>

namespace Dawn
{
	MAKE_TYPE_BEGIN(MeshFilterSystem)
	MAKE_TYPE_END()

	void MeshFilterSystem::Update(World* InWorld)
	{BROFILER_EVENT("Mesh Submission")

		// note (gb): the whole system is currently work in progress and should change
		//			  later on when we have better memory management (in case we need it).
		auto rs = g_Application->GetResourceSystem();
		auto renderer = g_Application->GetRenderer();
		auto* camera = World::GetActiveCamera();
		auto components = InWorld->GetComponentSets<MeshFilter, Transform>();

		for (std::pair<MeshFilter*, Transform*> pair : components)
		{
			auto* view = pair.first;
			auto* mesh = rs->FindMesh(view->MeshId);
			auto* transform = pair.second;
		
			// todo (gb): doesn't work at the moment (bounds wrong?!)
			//if (CameraUtils::IsBoxVisible(camera->GetFrustum(), mesh->Bounds.Min, mesh->Bounds.Max))
			//	continue;

			// todo (gb): this should only be done once for static geometry!
			
			auto* material = rs->FindMaterial(mesh->Materials[0]);

			auto Key = GenDrawKey64(true, material->Id.Index, RenderLayer::StaticGeom, 0.0f);

			auto DrawCmd = renderer->GetPass(RenderPassId::Geometry)->Bucket.AddCommand<Draw::DrawIndexedData>(Key);
			DrawCmd->IndexCount = mesh->NumIndices;
			DrawCmd->VertexArrayId = mesh->VertexArrayId;
			DrawCmd->Model = transform->WorldSpace;

			/*auto ShadowDrawCmd = renderer->PerFrameData.ShadowBucket.AddCommand<Draw::DrawIndexedData>(0u);
			ShadowDrawCmd->IndexCount = mesh->NumIndices;
			ShadowDrawCmd->VertexArrayId = mesh->VertexArrayId;
			ShadowDrawCmd->Model = transform->WorldSpace;*/
		}
	}
}