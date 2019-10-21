#pragma once
#include "inc_common.h"
#include "Core/Logging/Log.h"
#include "GfxResource.h"
#include "GfxImmediatePrimitives.h"
#include "Core/Container/HandleObjectArray.h"

namespace Dawn
{
	typedef HandleObjectSlot<GfxIndexBuffer> IndexBufferSlot;
	typedef HandleObjectSlot<GfxVertexBuffer> VertexBufferSlot;
	typedef HandleObjectSlot<GfxVertexArray> VertexArraySlot;
	typedef HandleObjectSlot<GfxShader> ShaderSlot;
	typedef HandleObjectSlot<GfxTexture> TextureSlot;
	typedef HandleObjectSlot<GfxRenderBuffer> RenderBufferSlot;

	class DAWN_API GfxGDI : public std::enable_shared_from_this<GfxGDI>
	{
	public:
		static GfxGDI* Create();

		virtual bool Init(const AppSettings& InSettings) = 0;

		virtual void Present() = 0;
		virtual void Shutdown() = 0;

	public:
		virtual void ActivateTextureSlot(u32 InIndex) = 0;
		virtual void DrawIndexed(GfxResId InVertexArrayId) = 0;
		virtual void DrawArray(GfxResId VertexArrayId) = 0;
		virtual void DrawInstanced(GfxResId VertexArrayId, u32 InAmount) = 0;
		void SetClearColor(const vec4& InColor);
		virtual void SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom) = 0;

		virtual void ClearWithColor(const vec4& InColor) = 0;
		virtual void Clear() = 0;

	public:
		virtual GfxResId CreateVertexBuffer(void* InVertices, u32 InSize, GfxVertexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateIndexBuffer(u32* InIndices, u32 InSize, GfxIndexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateVertexArray(GfxVertexArray** OutBuffer) = 0;
		virtual GfxResId CreateShader(GfxShader** OutShader) = 0;
		virtual GfxResId CreateTexture(const GfxTextureDesc& InDesc, GfxTexture** OutTexture) = 0;
		virtual GfxResId CreateRenderBuffer(GfxRenderBuffer** OutTexture) = 0;


		inline GfxVertexBuffer* GetVertexBuffer(GfxResId InId)
		{
			return VertexBufferPool.Find(InId);
		}

		inline GfxIndexBuffer* GetIndexBuffer(GfxResId InId)
		{
			return IndexBufferPool.Find(InId);
		}

		inline GfxVertexArray* GetVertexArray(GfxResId InId)
		{
			return VertexArrayPool.Find(InId);
		}

		inline GfxShader* GetShader(GfxResId InId)
		{
			return ShaderPool.Find(InId);
		}

		inline GfxTexture* GetTexture(GfxResId InId)
		{
			return TexturePool.Find(InId);
		}

		inline GfxRenderBuffer* GetRenderBuffer(GfxResId InId)
		{
			return RenderBufferPool.Find(InId);
		}

		inline GfxImmediatePrimitives* GetPrimitiveHelper()
		{
			return Primitives.get();
		}

		void ReturnVertexBuffer(GfxResId InId);
		void ReturnIndexBuffer(GfxResId InId);
		void ReturnVertexArray(GfxResId InId);
		void ReturnShader(GfxResId InId);
		void ReturnRenderBuffer(GfxResId InId);
	protected:
		Unique<GfxImmediatePrimitives> Primitives;
		HandleObjectArray<GfxVertexBuffer> VertexBufferPool;
		HandleObjectArray<GfxIndexBuffer> IndexBufferPool;
		HandleObjectArray<GfxVertexArray> VertexArrayPool;
		HandleObjectArray<GfxShader> ShaderPool;
		HandleObjectArray<GfxTexture> TexturePool;
		HandleObjectArray<GfxRenderBuffer> RenderBufferPool;

		vec4 ClearColor = vec4(1, 0, 0, 1);
	};

	typedef std::shared_ptr<GfxGDI> GfxGDIPtr;
}