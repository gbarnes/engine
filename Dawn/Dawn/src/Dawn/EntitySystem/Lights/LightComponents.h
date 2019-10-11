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
	};

	typedef std::shared_ptr<DirectionalLight> DirectionalLightPtr;

	struct DAWN_API PointLight : public Component<PointLight>
	{
		vec4 Color;
		float Range;
	};

	typedef std::shared_ptr<PointLight> PointLightPtr;

	struct DAWN_API SpotLight : public Component<SpotLight>
	{
		vec4 Color;
		float Angle;
		float Range;
	};

	typedef std::shared_ptr<SpotLight> SpotLightPtr;

	class LightUtils
	{
	public:
		static DirectionalLight* CreateDirectionalLight(World* InWorld,  quat InOrientation, vec4 InColor);
		static PointLight* CreatePointLight(vec3 InPosition, vec4 InColor, float InRange);
		static SpotLight* CreateSpotLight(vec3 InPosition, quat InOrientation, vec4 InColor, float InAngle, float InRange);
	};
}