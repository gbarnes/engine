#pragma once
#include "../System.h"
#include "LightComponents.h"
#include "Rendering/Renderer.h"

namespace Dawn
{
	class DAWN_API LightSystem
	{
	public:
		static void Update(World* InWorld, GfxShader* InShader)
		{
			BROFILER_EVENT("LightSystem Update");

			
			u32 i = 0;
			auto pointLights = InWorld->GetComponentSets<PointLight, Transform>();
			for (std::pair<PointLight*, Transform*> pair : pointLights)
			{
				auto entity = pair.second->GetEntity();
				if (entity->bIsActive)
				{
					InShader->SetVec3("pointLights[" + std::to_string(i) + "].position", pair.second->Position);
					InShader->SetVec3("pointLights[" + std::to_string(i) + "].color", pair.first->Color);
					InShader->SetFloat("pointLights[" + std::to_string(i) + "].intensity", pair.first->Intensity);
					InShader->SetFloat("pointLights[" + std::to_string(i) + "].range", pair.first->Range);
					++i;
				}
				
			}
			InShader->SetInt("pointLightNum", i);

			i = 0;
			auto directionalLights = InWorld->GetComponentSets<DirectionalLight, Transform>();
			for (std::pair<DirectionalLight*, Transform*> pair : directionalLights)
			{
				auto entity = pair.second->GetEntity();
				if (entity->bIsActive) 
				{
					TransformUtils::CalculateForward(pair.second);
					InShader->SetVec3("directionalLights[" + std::to_string(i) + "].direction", pair.second->Forward);
					InShader->SetVec3("directionalLights[" + std::to_string(i) + "].color", pair.first->Color);
					InShader->SetFloat("directionalLights[" + std::to_string(i) + "].intensity", pair.first->Intensity);
					InShader->SetMat4("directionalLights[" + std::to_string(i) + "].lightSpace", pair.first->LightSpace);
					++i;
				}
				
			}

			InShader->SetInt("directionalLightsNum", i);
		}
	};

}