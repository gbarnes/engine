#pragma once

#include "../GfxResource.h"

namespace Dawn
{
	class GLVertexBuffer : public GfxVertexBuffer
	{
	public:
		GLVertexBuffer(GfxResId InId)
			: GfxVertexBuffer(InId) {};
		virtual ~GLVertexBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Reset(const GfxBufferLayout& InLayout, float* InData, u32 InSize) override;
		virtual const GfxBufferLayout& GetLayout() const override;

	protected:
		GfxBufferLayout Layout;
		u32 RendererId;
	};

	class GLIndexBuffer : public GfxIndexBuffer
	{
	public:
		GLIndexBuffer(GfxResId InId);
		virtual ~GLIndexBuffer();

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void Reset(u16* InData, u32 InSize) override;

	protected:
		u32 RendererId;
	};

	class GLVertexArray : public GfxVertexArray
	{
	public:
		GLVertexArray(GfxResId InId);

		virtual void Bind() override;
		virtual void Unbind() override;
		void AttachVertexBuffer(const std::shared_ptr<GfxVertexBuffer>& InBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<GfxIndexBuffer>& InBuffer) override;

	private:
		u32 RendererId;
		std::vector<std::shared_ptr<GfxVertexBuffer>> Vertices;
		std::shared_ptr<GfxIndexBuffer> Indices;
	};

}