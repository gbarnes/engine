#pragma once
#include "inc_common.h"
#include "Core/Logging/Log.h"
#include "GfxResource.h"

namespace Dawn
{
	template<typename T>
	class GfxResourcePool;

	template<typename T>
	struct DAWN_API GfxResourceBundle
	{
		T* Element;

		GfxResId GetId() const
		{
			return Id;
		}

		bool IsValid()
		{
			return Element != nullptr;
		}

	private:
		GfxResId Id;

		friend class GfxResourcePool<T>;
	};

	typedef GfxResourceBundle<GfxIndexBuffer> IndexBufferBundle;
	typedef GfxResourceBundle<GfxVertexBuffer> VertexBufferBundle;
	typedef GfxResourceBundle<GfxVertexArray> VertexArrayBundle;
	typedef GfxResourceBundle<GfxShader> ShaderBundle;
	typedef GfxResourceBundle<GfxTexture> TextureBundle;

	template<typename T>
	class DAWN_API GfxResourcePool
	{
	public:
		GfxResourcePool()
		{
			FreeId.IsValid = true;
			FreeId.Generation = 0;
			FreeId.Index = 0;
		}

		GfxResourceBundle<T>* Request()
		{
			/*if (Resources.size() > FreeId.Index)
			{
				auto& Bundle = Resources[FreeId.Index];
				if (!Bundle.IsValid()) 
				{
					Bundle.Id.Generation++;
					return &Bundle;
				}
			}*/

			GfxResourceBundle<T> Bundle;
			Bundle.Id.Index = (u32)(Resources.size() - 1);
			Bundle.Id.Generation = 1;
			Bundle.Id.IsValid = true;

			FreeId.Index = ++Bundle.Id.Index;
			FreeId.Generation = 0;

			return &Resources.emplace_back(Bundle);
		}

		T* Get(GfxResId Id)
		{
			if (!Id.IsValid)
				return nullptr;

			auto& Element = Resources[Id.Index];

			if (Element.Id.Generation != Id.Generation)
				DWN_CORE_WARN("The generation of the passed id doesn't matches with the id in the pool.");

			return Resources[Id.Index].Element;
		}

		void Free(GfxResId Id)
		{
			if (!Id.IsValid)
				return;

			FreeId = Id;
			auto& Bundle = Resources[Id.Index];

			delete Bundle.Element;
			Bundle.Element = nullptr;
		}

		void Clear()
		{
			for (auto Bundle : Resources)
			{
				if (Bundle.Element != nullptr) 
				{
					delete Bundle.Element;
					Bundle.Element = nullptr;
				}
			}

			Resources.clear();

			FreeId.IsValid = true;
			FreeId.Generation = 0;
			FreeId.Index = 0;
		}

	private:
		GfxResId FreeId;
		std::vector<GfxResourceBundle<T>> Resources;
	};


	class GfxImmediatePrimitives;

	class DAWN_API GfxGDI
	{
	public:
		inline static GfxGDI* Get()
		{
			return Instance;
		}

		static GfxGDI* Create();

		virtual bool Init(const AppSettings& InSettings) = 0;

		virtual void Present() = 0;
		virtual void Shutdown() = 0;

	public:
		virtual void ActivateTextureSlot(u32 InIndex) = 0;
		virtual void DrawIndexed(GfxResId InVertexArrayId) = 0;
		void SetClearColor(const vec4& InColor);
		virtual void SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom) = 0;

		virtual void Clear() = 0;

	public:
		virtual GfxResId CreateVertexBuffer(float* InVertices, u32 InSize, GfxVertexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateIndexBuffer(u32* InIndices, u32 InSize, GfxIndexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateVertexArray(GfxVertexArray** OutBuffer) = 0;
		virtual GfxResId CreateShader(GfxShader** OutShader) = 0;
		virtual GfxResId CreateTexture(u8* InData, u32 InWidth, u32 InHeight, u16 InChannelsPerPixel, GfxWrapDesc InWrap,
			GfxFilterDesc InFilter, bool InGenerateMipMaps, GfxTexture** OutTexture) = 0;


		inline GfxVertexBuffer* GetVertexBuffer(GfxResId InId)
		{
			return VertexBufferPool.Get(InId);
		}

		inline GfxIndexBuffer* GetIndexBuffer(GfxResId InId)
		{
			return IndexBufferPool.Get(InId);
		}

		inline GfxVertexArray* GetVertexArray(GfxResId InId)
		{
			return VertexArrayPool.Get(InId);
		}

		inline GfxShader* GetShader(GfxResId InId)
		{
			return ShaderPool.Get(InId);
		}

		inline GfxTexture* GetTexture(GfxResId InId)
		{
			return TexturePool.Get(InId);
		}

		void ReturnVertexBuffer(GfxResId InId);
		void ReturnIndexBuffer(GfxResId InId);
		void ReturnVertexArray(GfxResId InId);
		void ReturnShader(GfxResId InId);

	protected:
		GfxResourcePool<GfxVertexBuffer> VertexBufferPool;
		GfxResourcePool<GfxIndexBuffer> IndexBufferPool;
		GfxResourcePool<GfxVertexArray> VertexArrayPool;
		GfxResourcePool<GfxShader> ShaderPool;
		GfxResourcePool<GfxTexture> TexturePool;

		vec4 ClearColor = vec4(1, 0, 0, 1);
	private:
		static GfxGDI* Instance;
	};
}