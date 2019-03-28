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
		ShaderHandle ID_PhongVS;
		ShaderHandle ID_PhongPS;
		ShaderHandle ID_DebugVS;
		ShaderHandle ID_DebugPS;
	};

	namespace RenderResourceHelper
	{
		static CommonShaderHandles CommonShaders;
		void LoadCommonShaders();
		void CreateCommonPipelineStates();
	}
}