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
	

	void RenderResourceHelper::LoadCommonShaders(ResourceSystem* InResourceSystem)
	{
		// Create the common shader resources in the order 
		// the enum is structured!
		CommonShaderHandles::DebugPrim = InResourceSystem->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = InResourceSystem->LoadFile("Shader/debug_gl.shader");
		EditorShaderHandles::Grid = InResourceSystem->LoadFile("Shader/editor_grid.shader");
		//CommonShaders.ID_DebugPS = rs->LoadFile("Shader/debug_ps.glsl");
	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}