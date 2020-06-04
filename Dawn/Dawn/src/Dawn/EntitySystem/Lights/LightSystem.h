#pragma once
#include "../System.h"
#include "LightComponents.h"
#include "EntitySystem/Camera/Camera.h"
#include "Rendering/Renderer.h"

namespace Dawn
{
	class DAWN_API LightSystem
	{
	public:
		static void Update(World* InWorld)
		{
			BROFILER_EVENT("LightSystem Update");

			CBPointLightData plData = {};

			u32 i = 0;
			auto pointLights = InWorld->GetComponentSets<PointLight, Transform>();
			for (std::pair<PointLight*, Transform*> pair : pointLights)
			{
				auto entity = pair.second->GetEntity();
				auto meta = InWorld->GetEntityMetaData(entity);

				if (meta->bIsActive)
				{
					plData.Position[i] = pair.second->Position;
					plData.Color[i] = pair.first->Color;
					plData.Intensity[i] = pair.first->Intensity;
					plData.Range[i] = pair.first->Range;
					++i;
				}
				
			}
			
			plData.Num = i;

			// note (gb): this will always take the last directional light that is active!
			//			  find a better solution for this?!
			CBDirLightData dirData = {};
			auto directionalLights = InWorld->GetComponentSets<DirectionalLight, Transform>();
			for (std::pair<DirectionalLight*, Transform*> pair : directionalLights)
			{
				auto entity = pair.second->GetEntity();
				auto meta = InWorld->GetEntityMetaData(entity);

				if (meta->bIsActive)
				{
					TransformUtils::CalculateForward(pair.second);

					LightUtils::CalculateOrthoLightMatrix(InWorld, pair.first, World::GetActiveCamera()->NearZ, World::GetActiveCamera()->FarZ);

					dirData.Direction = pair.second->Forward;
					dirData.Color = pair.first->Color;
					dirData.Intensity = pair.first->Intensity;
					dirData.LightSpace = pair.first->LightSpace;
				}
				
			}

			//g_Application->GetGDI()->UpdateConstantBuffer(CommonConstantBuffers::PointLightData, &plData, sizeof(plData));
			g_Application->GetGDI()->UpdateConstantBuffer(CommonConstantBuffers::DirLightData, &dirData, sizeof(dirData));
		}
	};

}