#pragma once
#include "inc_common.h"
#include "GfxResource.h"

namespace Dawn
{
	class DAWN_API GfxGDI
	{
	public:

		virtual bool Init(const AppSettings& InSettings) = 0;

		virtual void Present() = 0;
		virtual void Shutdown() = 0;

	public:
		virtual GfxResId CreateVertexBuffer(std::shared_ptr<GfxVertexBuffer>* OutBuffer) = 0;
		virtual GfxResId CreateIndexBuffer(std::shared_ptr<GfxIndexBuffer>* OutBuffer) = 0;
		virtual GfxResId CreateVertexArray(GfxVertexArray* OutBuffer) = 0;
	};
}