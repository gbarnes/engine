#include "RenderResourceHelper.h"
#include "Core/GDI/inc_gfx_types.h"
#include "ResourceSystem/ResourceSystem.h"
#include "ResourceSystem/ResourceTable.h"

namespace Dawn 
{
	void RenderResourceHelper::LoadCommonShaders()
	{
		// Create the common shader resources in the order 
		// the enum is structured!
		RefPtr<ResourceSystem> rs(ResourceSystem::Get());
		//CommonShaders.ID_PhongVS = rs->LoadFile("Shader/phong_vs.glsl");
		//CommonShaders.ID_PhongPS = rs->LoadFile("Shader/phong_ps.glsl");

		CommonShaders.ID_Debug = rs->LoadFile("Shader/debug_gl.shader");
		//CommonShaders.ID_DebugPS = rs->LoadFile("Shader/debug_ps.glsl");
	}

	void RenderResourceHelper::CreateCommonPipelineStates()
	{
		
	}
}