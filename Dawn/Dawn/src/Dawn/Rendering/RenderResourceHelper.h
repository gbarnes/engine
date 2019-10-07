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
		static ShaderHandle Phong;
		static ShaderHandle Debug;
		static ShaderHandle DebugPrim;
	};

	struct EditorShaderHandles
	{
		static ShaderHandle Grid;
	};

	class ResourceSystem;

	namespace RenderResourceHelper
	{
		void LoadCommonShaders(ResourceSystem* InResourceSystem);
		void CreateCommonPipelineStates();
	}
}