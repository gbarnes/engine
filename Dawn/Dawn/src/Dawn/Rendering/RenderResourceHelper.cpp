#include "RenderResourceHelper.h"
#include "Core/GDI/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"

namespace Dawn 
{
	ResourceId CommonShaderHandles::FinalPass;
	ResourceId CommonShaderHandles::LightingPass;
	ResourceId CommonShaderHandles::DebugPrim;
	ResourceId CommonShaderHandles::Phong;
	ResourceId CommonShaderHandles::Debug;
	ResourceId EditorShaderHandles::Grid;
	ResourceId CommonShaderHandles::Standard;
	ResourceId CommonShaderHandles::StandardInstanced;
	ResourceId CommonShaderHandles::SSAOCompute;


	void RenderResourceHelper::LoadCommonShaders(ResourceSystem* InResourceSystem)
	{
		CommonShaderHandles::FinalPass = InResourceSystem->LoadFile("Shader/final_pass.shader");
		CommonShaderHandles::LightingPass = InResourceSystem->LoadFile("Shader/lighting_pass.shader");
		CommonShaderHandles::Standard = InResourceSystem->LoadFile("Shader/standard.shader");
		CommonShaderHandles::StandardInstanced = InResourceSystem->LoadFile("Shader/standard_instanced.shader");
		CommonShaderHandles::DebugPrim = InResourceSystem->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = InResourceSystem->LoadFile("Shader/debug_gl.shader");
		CommonShaderHandles::SSAOCompute = InResourceSystem->LoadFile("Shader/ssao_compute.shader");
		EditorShaderHandles::Grid = InResourceSystem->LoadFile("Shader/editor_grid.shader");

	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}