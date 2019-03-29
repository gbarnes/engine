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
		ShaderHandle ID_Phong;
		ShaderHandle ID_Debug;
	};

	namespace RenderResourceHelper
	{
		static CommonShaderHandles CommonShaders;
		void LoadCommonShaders();
		void CreateCommonPipelineStates();
	}
}