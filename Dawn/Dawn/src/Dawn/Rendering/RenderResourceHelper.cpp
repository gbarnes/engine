#include "RenderResourceHelper.h"
#include "Core/GDI/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"

namespace Dawn 
{
	ResourceId CommonShaderHandles::DebugPrim;
	ResourceId CommonShaderHandles::Phong;
	ResourceId CommonShaderHandles::Debug;
	ResourceId EditorShaderHandles::Grid;
	ResourceId CommonShaderHandles::Standard;
	

	void RenderResourceHelper::LoadCommonShaders(ResourceSystem* InResourceSystem)
	{
		// Create the common shader resources in the order 
		// the enum is structured!
		CommonShaderHandles::Standard = InResourceSystem->LoadFile("Shader/standard.shader");
		CommonShaderHandles::DebugPrim = InResourceSystem->LoadFile("Shader/debug_prim.shader");
		CommonShaderHandles::Debug = InResourceSystem->LoadFile("Shader/debug_gl.shader");
		EditorShaderHandles::Grid = InResourceSystem->LoadFile("Shader/editor_grid.shader");
		//CommonShaders.ID_DebugPS = rs->LoadFile("Shader/debug_ps.glsl");
	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}