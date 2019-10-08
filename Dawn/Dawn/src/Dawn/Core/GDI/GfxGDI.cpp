#include "GfxGDI.h"
#include "OpenGL/GfxGLGDI.h"

namespace Dawn
{
	GfxGDI* GfxGDI::Create()
	{
		//#ifdef USE_OPENGL_GFX
		GfxGDI* device = new GfxGLGDI();
		return device;
		//#endif
	}

	void GfxGDI::ReturnVertexBuffer(GfxResId Id)
	{
		this->VertexBufferPool.Free(Id);
	}
	
	void GfxGDI::ReturnIndexBuffer(GfxResId Id)
	{
		this->IndexBufferPool.Free(Id);
	}
	
	void GfxGDI::ReturnVertexArray(GfxResId Id)
	{
		this->VertexArrayPool.Free(Id);
	}

	void GfxGDI::ReturnShader(GfxResId InId)
	{
		this->ShaderPool.Free(InId);
	}

	void GfxGDI::SetClearColor(const vec4& InColor)
	{
		this->ClearColor = InColor;
	}
}