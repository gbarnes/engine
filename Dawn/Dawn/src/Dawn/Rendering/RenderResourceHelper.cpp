#include "RenderResourceHelper.h"
#include "Core/GDI/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceTable.h"

namespace Dawn 
{
	ShaderHandle CommonShaderHandles::DebugPrim;
	ShaderHandle CommonShaderHandles::Phong;
	ShaderHandle CommonShaderHandles::Debug;
	ShaderHandle EditorShaderHandles::Grid;
	

	void RenderResourceHelper::LoadCommonShaders()
	{
		// Create the common shader resources in the order 
		// the enum is structured!
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());
		//CommonShaders.ID_PhongVS = rs->LoadFile("Shader/phong_vs.glsl");
		//CommonShaders.ID_PhongPS = rs->LoadFile("Shader/phong_ps.glsl");

		CommonShaderHandles::DebugPrim = rs->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = rs->LoadFile("Shader/debug_gl.shader");
		EditorShaderHandles::Grid = rs->LoadFile("Shader/editor_grid.shader");
		//CommonShaders.ID_DebugPS = rs->LoadFile("Shader/debug_ps.glsl");
	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}