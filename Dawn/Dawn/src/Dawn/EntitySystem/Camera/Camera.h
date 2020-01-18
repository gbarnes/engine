#pragma once
#include "../Component.h"
#include "Core/Type.h"


namespace Dawn
{
	class CameraSystem;
	class CameraUtils;
	struct Transform;
	class World;

	// Let this only be a normal struct containing plain data
	// without any methods and add the methods to 
	// the camera system and make it the main access point for cameras ?
	// 
	struct DAWN_API Camera : public Component<Camera>
	{
		REGISTER_TYPE(Camera)

		friend class CameraUtils;
		
		static const u32 Version = 1;
		float FieldOfView = 65.0f;
		float NearZ = 0.1f;
		float FarZ = 100.0f;
		float AspectRatio = 0;
		u32 Width = 800;
		u32 Height = 600;
		bool bIsOrthographic = false;

		vec4 ClearColor = { 0.4f, 0.6f, 0.9f, 1.0f };
		vec3 WorldUp = {0.0f,1.0f,0.0f};

		Transform* GetTransform(World* InWorld);
		const mat4& GetProjection() const;
		const mat4& GetView() const;
		static void InitFromLoad(World* InWorld, void* Component);

	private:
		
		mat4 Projection = mat4(1);
		mat4 View = mat4(1);
	};


	//
	// Helper function to get a camera from the world!
	//
	extern Camera* GetCamera(World* InWorld, u32 i);

	//
	// Helper function to create a new camera at the world!
	//
	extern Camera* CreateCamera(World* InWorld, std::string&& InName, u32 Width, u32 Height,
		float InNearZ, float InFarZ, float InFoV, vec4 InClearColor,
		const vec3& InPosition = vec3(0), const quat& InOrientation = quat());

	class CameraUtils
	{
	public:
		//
		// Calculates the view matrix from the given orientation and translation matrix
		//
		static const mat4& CalculateView(Camera* InCamera, Transform* InTransform);

		//
		// Calculates the frustum of the camera used for visibility check
		//
		static const void CalculateFrustum(Camera* InCamera);

		//
		// Calculates the perspective matrix for the given camera.
		//
		static const mat4& CalculatePerspective(Camera* InCamera);

		//
		// Calculates the orthographic matrix for the given camera.
		//
		static const mat4& CalculateOthographic(Camera* InCamera);

		//
		// Converts a world point to screenspace one
		//
		static vec2 WorldToScreenPoint(Camera* InCamera, const vec3& InPoint);

		//
		// Converts a screenspace point to worldspace one
		//
		static vec3 ScreenToWorldPoint(Camera* InCamera, const vec3& InPoint);

	};
}