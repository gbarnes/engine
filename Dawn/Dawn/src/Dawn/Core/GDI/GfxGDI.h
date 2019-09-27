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
			if (Resources.size() > FreeId.Index)
			{
				auto& Bundle = Resources[FreeId.Index];
				if (!Bundle.IsValid()) 
				{
					Bundle.Id.Generation++;
					return &Bundle;
				}
			}

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
		virtual void DrawIndexed(GfxResId VertexArrayId) = 0;



	public:
		virtual GfxResId CreateVertexBuffer(float* Vertices, u32 Size, GfxVertexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateIndexBuffer(u16* Indices, u32 Size, GfxIndexBuffer** OutBuffer) = 0;
		virtual GfxResId CreateVertexArray(GfxVertexArray** OutBuffer) = 0;
		virtual GfxResId CreateShader(GfxShader** OutShader) = 0;
		virtual GfxResId CreateTexture(u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
			GfxFilterDesc Filter, bool GenerateMipMaps, GfxTexture** OutTexture) = 0;


		inline GfxVertexBuffer* GetVertexBuffer(GfxResId Id)
		{
			return VertexBufferPool.Get(Id);
		}

		inline GfxIndexBuffer* GetIndexBuffer(GfxResId Id)
		{
			return IndexBufferPool.Get(Id);
		}

		inline GfxVertexArray* GetVertexArray(GfxResId Id)
		{
			return VertexArrayPool.Get(Id);
		}

		inline GfxShader* GetShader(GfxResId Id)
		{
			return ShaderPool.Get(Id);
		}

		inline GfxTexture* GetTexture(GfxResId Id)
		{
			return TexturePool.Get(Id);
		}

		void ReturnVertexBuffer(GfxResId Id);
		void ReturnIndexBuffer(GfxResId Id);
		void ReturnVertexArray(GfxResId Id);
		void ReturnShader(GfxResId Id);

	protected:
		GfxResourcePool<GfxVertexBuffer> VertexBufferPool;
		GfxResourcePool<GfxIndexBuffer> IndexBufferPool;
		GfxResourcePool<GfxVertexArray> VertexArrayPool;
		GfxResourcePool<GfxShader> ShaderPool;
		GfxResourcePool<GfxTexture> TexturePool;

	private:
		static GfxGDI* Instance;
	};
}