#include "GfxGDI.h"
#include "OpenGL/GfxGLGDI.h"

namespace Dawn
{
	GfxGDI* GfxGDI::Instance = nullptr;
	GfxGDI* GfxGDI::Create()
	{
		if (Instance != nullptr)
		{
			DWN_CORE_ERROR("Warning you're trying to create the GDI twice!");
			return Instance;
		}

		//#ifdef USE_OPENGL_GFX
		GfxGDI* device = new GfxGLGDI();
		Instance = device;
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

	void GfxGDI::SetClearColor(const vec4& InColor)
	{
		this->ClearColor = InColor;
	}
}