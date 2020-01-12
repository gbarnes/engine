#pragma once
#include "inc_common.h"
#include "../Component.h"
#include "../Transform/Transform.h"
#include "../World.h"

namespace Dawn
{
	struct DAWN_API DirectionalLight : public Component<DirectionalLight>
	{
		REGISTER_TYPE(DirectionalLight)
		vec4 Color;
		float Intensity = 1.0f;
		bool bCastShadows = true;
		mat4 LightSpace;

		static void InitFromLoad(World* InWorld, void* Component)
		{
			auto light = static_cast<DirectionalLight*>(Component);
			light->WorldRef = InWorld;
		}
	};

	struct DAWN_API PointLight : public Component<PointLight>
	{
		REGISTER_TYPE(PointLight)
		vec4 Color;
		float Range = 1.0f;
		float Intensity = 1.0f;
		bool bCastShadows = false;

		static void InitFromLoad(World* InWorld, void* Component)
		{
			auto light = static_cast<PointLight*>(Component);
			light->WorldRef = InWorld;
		}
	};

	struct DAWN_API SpotLight : public Component<SpotLight>
	{
		//REGISTER_TYPE(SpotLight)
		vec4 Color;
		float Angle;
		float Range;
		bool bCastShadows = false;

		static void InitFromLoad(World* InWorld, void* Component)
		{
			auto light = static_cast<SpotLight*>(Component);
			light->WorldRef = InWorld;
		}
	};

	class LightUtils
	{
	public:
		static DirectionalLight* CreateDirectionalLight(World* InWorld, quat InOrientation, vec4 InColor, float InIntensity = 1.0f);
		static PointLight* CreatePointLight(World* InWorld, vec3 InPosition, vec4 InColor, float InIntensity, float InRange);
		static SpotLight* CreateSpotLight(vec3 InPosition, quat InOrientation, vec4 InColor, float InAngle, float InRange);


		static void CalculateOrthoLightMatrix(World* InWorld, DirectionalLight* InLight, float InNearPlane, float InFarPlane);
	};
}