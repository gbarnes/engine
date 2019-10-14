#pragma once
#include "ResourceSystem/Resources.h"

namespace Dawn
{
	enum RootSignatureTypes {
		RS_Standard,
		RS_Debug
	};

	enum PipelineStateTypes {
		PS_Standard,
		PS_Debug_Line,
		PS_Debug_Point
	};

	struct DAWN_API CommonShaderHandles
	{
		static ResourceId FinalPass;
		static ResourceId Standard;
		static ResourceId Phong;
		static ResourceId Debug;
		static ResourceId DebugPrim;
	};

	struct DAWN_API EditorShaderHandles
	{
		static ResourceId Grid;
	};

	class ResourceSystem;

	namespace RenderResourceHelper
	{
		DAWN_API void LoadCommonShaders(ResourceSystem* InResourceSystem);
		DAWN_API void CreateCommonPipelineStates();
	}
}