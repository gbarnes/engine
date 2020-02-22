#include "stdafx.h"
#include "RenderResourceHelper.h"
#include "Core/GDI/Base/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"

namespace Dawn 
{
	ResourceId CommonShaderHandles::FXAA;
	ResourceId CommonShaderHandles::LightingPass;
	ResourceId CommonShaderHandles::DebugPrim;
	ResourceId CommonShaderHandles::Phong;
	ResourceId CommonShaderHandles::Debug;
	ResourceId EditorShaderHandles::Grid;
	ResourceId CommonShaderHandles::Standard;
	ResourceId CommonShaderHandles::StandardInstanced;
	ResourceId CommonShaderHandles::SSAOCompute;
	ResourceId CommonShaderHandles::SSAOBlur;
	ResourceId CommonShaderHandles::ShadowMapCompute;

	ResourceId CommonShaderHandles::DebugVS;
	ResourceId CommonShaderHandles::DebugPS;

	void RenderResourceHelper::LoadCommonShaders(ResourceSystem* InResourceSystem)
	{
		/*CommonShaderHandles::FXAA = InResourceSystem->LoadFile("Shader/fxaa.shader");
		CommonShaderHandles::LightingPass = InResourceSystem->LoadFile("Shader/lighting_pass.shader");
		CommonShaderHandles::Standard = InResourceSystem->LoadFile("Shader/standard.shader");
		CommonShaderHandles::StandardInstanced = InResourceSystem->LoadFile("Shader/standard_instanced.shader");
		CommonShaderHandles::DebugPrim = InResourceSystem->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = InResourceSystem->LoadFile("Shader/debug_gl.shader");
		CommonShaderHandles::SSAOCompute = InResourceSystem->LoadFile("Shader/ssao_compute.shader");
		CommonShaderHandles::SSAOBlur = InResourceSystem->LoadFile("Shader/ssao_blur.shader");
		EditorShaderHandles::Grid = InResourceSystem->LoadFile("Shader/editor_grid.shader");
		CommonShaderHandles::ShadowMapCompute = InResourceSystem->LoadFile("Shader/shadowmap_compute.shader");*/


		CommonShaderHandles::DebugVS = InResourceSystem->LoadFile("Shader/HLSL/default.h_vs");
		CommonShaderHandles::DebugPS = InResourceSystem->LoadFile("Shader/HLSL/default.h_ps");
	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}