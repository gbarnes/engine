#include "stdafx.h"
#include "GfxGDI.h"

#include "../OpenGL/GfxGLGDI.h"
#include "../Vulkan/GfxVulkanGDI.h"
#include "../DX11/DX11GDI.h"

#include "GfxResource.h"
#include "GfxTexture.h"
#include "GfxShader.h"
#include "GfxPipelineStateObject.h"
#include "GfxBuffer.h"
#include "GfxVertexArrayObject.h"
#include "GfxSampler.h"

#define GFX_FREE_OBJECTS(ReturnMethod, PoolName) auto PoolName##s = PoolName##.GetValues();\
for (auto* PoolName##Obj : PoolName##s){\
	ReturnMethod##(PoolName##Obj->GetId());\
}\
PoolName##.Clear();\

namespace Dawn
{
	GfxGDI* GfxGDI::Create()
	{
		//#ifdef USE_OPENGL_GFX
		//GfxGDI* device = new GfxGLGDI();
		//GfxGDI* device = new GfxVulkanGDI();
		GfxGDI* device = new DX11GDI();
		return device;
		//#endif

		//return nullptr;
	}

	void GfxGDI::Shutdown()
	{
		GFX_FREE_OBJECTS(ReturnVertexArrayObject, VertexArrayObjectPool);
		GFX_FREE_OBJECTS(ReturnSampler, SamplerPool);
		GFX_FREE_OBJECTS(ReturnBuffer, BufferPool);
		GFX_FREE_OBJECTS(ReturnTextureView, TextureViewPool);
		GFX_FREE_OBJECTS(ReturnTexture, TexturePool);
		GFX_FREE_OBJECTS(ReturnPipelineState, PipelineStatePool);
		GFX_FREE_OBJECTS(ReturnShader, ShaderPool);
	}


	void GfxGDI::ReturnTextureView(const GfxResId& InId)
	{
		static_cast<GfxTextureView*>(TextureViewPool.Find(InId))->~GfxTextureView();
		this->TextureViewPool.Free(InId);
	}

	void GfxGDI::ReturnTexture(const GfxResId& InId)
	{
		static_cast<GfxTexture*>(TexturePool.Find(InId))->~GfxTexture();
		this->TexturePool.Free(InId);
	}

	void GfxGDI::ReturnBuffer(const GfxResId& Id)
	{
		static_cast<GfxBuffer*>(BufferPool.Find(Id))->~GfxBuffer();
		this->BufferPool.Free(Id);
	}
	
	void GfxGDI::ReturnVertexArrayObject(const GfxResId& Id)
	{
		static_cast<GfxVertexArrayObject*>(VertexArrayObjectPool.Find(Id))->~GfxVertexArrayObject();
		this->VertexArrayObjectPool.Free(Id);
	}

	void GfxGDI::ReturnShader(const GfxResId& InId)
	{
		static_cast<GfxShader*>(ShaderPool.Find(InId))->~GfxShader();
		this->ShaderPool.Free(InId);
	}

	void GfxGDI::ReturnPipelineState(const GfxResId& InId)
	{
		static_cast<GfxPipelineStateObject*>(PipelineStatePool.Find(InId))->~GfxPipelineStateObject();
		this->PipelineStatePool.Free(InId);
	}

	void GfxGDI::ReturnSampler(const GfxResId& InId)
	{
		static_cast<GfxSampler*>(SamplerPool.Find(InId))->~GfxSampler();
		this->SamplerPool.Free(InId);
	}

	void GfxGDI::SetClearColor(const vec4& InColor)
	{
		this->ClearColor = InColor;
	}
}