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

	struct CommonShaderHandles
	{
		static ResourceId Standard;
		static ResourceId Phong;
		static ResourceId Debug;
		static ResourceId DebugPrim;
	};

	struct EditorShaderHandles
	{
		static ResourceId Grid;
	};

	class ResourceSystem;

	namespace RenderResourceHelper
	{
		void LoadCommonShaders(ResourceSystem* InResourceSystem);
		void CreateCommonPipelineStates();
	}
}