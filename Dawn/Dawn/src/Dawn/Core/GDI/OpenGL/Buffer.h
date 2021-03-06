#pragma once

#include "../GfxResource.h"

namespace Dawn
{
	class GfxGDI;

	class GLVertexBuffer : public GfxVertexBuffer
	{
	public:

		GLVertexBuffer()
			: GfxVertexBuffer(GfxResId()) {};

		GLVertexBuffer(void* Vertices, u32 Size, GfxResId InId);
		virtual ~GLVertexBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Reset(void* InData, u32 InSize) override;
		virtual const GfxBufferLayout& GetLayout() const override;
		virtual void SetLayout(GfxBufferLayout& InLayout) override;

	protected:
		GfxBufferLayout Layout;
		u32 RendererId;
	};

	class GLIndexBuffer : public GfxIndexBuffer
	{
	public:
		GLIndexBuffer()
			: GfxIndexBuffer(GfxResId()) {};

		GLIndexBuffer(u32* InData, u32 InSize, GfxResId InId);
		virtual ~GLIndexBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Reset(u32* InData, u32 InSize) override;

	protected:
		u32 RendererId;
	};

	class GLVertexArray : public GfxVertexArray
	{
	public:
		GLVertexArray()
			: GLVertexArray(GfxResId()) {};

		GLVertexArray( GfxResId InId);

		~GLVertexArray();

		void Bind() override;
		void Unbind() override;
		void AttachVertexBuffer(GfxVertexBuffer* InBuffer, i32 InDivisor = -1) override;
		void SetIndexBuffer(GfxIndexBuffer* InBuffer) override;
		void SetName(std::string Name) override;
		GfxVertexBuffer* GetVertexBuffer(GfxGDI* InGDI, u32 InIndex) override;
		GfxIndexBuffer* GetIndexBuffer(GfxGDI* InGDI) override;

		u32 GetInternalId() const
		{
			return RendererId;
		}

	private:
		u32 VertexBufferIndex = 0;
		u32 RendererId;
		std::vector<GfxResId> Vertices;
		GfxResId Indices;
	};
}