#pragma once
#include "ResourceSystem/Resources.h"

namespace Dawn
{
	struct DAWN_API CommonShaderHandles
	{
		static ResourceId FXAA;
		static ResourceId Standard;
		static ResourceId StandardInstanced;
		static ResourceId LightingPass;
		static ResourceId Phong;
		static ResourceId Debug;
		static ResourceId DebugPrim;
		static ResourceId SSAOCompute;
		static ResourceId SSAOBlur;
		static ResourceId ShadowMapCompute;

		static ResourceId DebugVS;
		static ResourceId DebugPS;
		static ResourceId ShadowMapVS;
		static ResourceId ColoredSimplePS;
	};

	struct DAWN_API EditorShaderHandles
	{
		static ResourceId Grid;
	};

	struct CommonConstantBuffers
	{
		static GfxResId PerAppData;
		static GfxResId PerObjectData;
		static GfxResId PerFrameData;
	};

	struct CBPerAppData
	{
		mat4 Projection;
	};

	struct CBPerFrameData
	{
		mat4 View;
	};

	struct CBPerObjectData
	{
		mat4 World;
	};

	enum RenderCachedPSO
	{
		PSO_Debug,
		PSO_Shadow,
		PSO_Num
	};

	class ResourceSystem;
	class Application;

	namespace RenderResourceHelper
	{
		

		DAWN_API void LoadCommonShaders(ResourceSystem* InResourceSystem);
		DAWN_API void CreateConstantBuffers(GfxGDI* InGDI);
		DAWN_API GfxResId CreateConstantBuffer(GfxGDI* InGDI, void* InData, i32 InSize);

		DAWN_API void CreatePSODefaults(Application* InApplication);
		DAWN_API void CachePSO(const std::string& InName, Dawn::GfxResId InId);
		DAWN_API Dawn::GfxResId GetCachedPSO(const std::string& InName);
	}
}