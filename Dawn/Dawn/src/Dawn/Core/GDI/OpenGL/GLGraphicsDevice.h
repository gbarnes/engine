#pragma once
#include "../GfxGDI.h"

//#ifdef USE_OPENGL_GFX
#include "inc_opengl.h"
#include "Buffer.h"

namespace Dawn
{
	class DAWN_API GLGraphicsDevice : public GfxGDI
	{
	public:
		bool Init(const AppSettings& InSettings);
		void Present();
		void Shutdown();
	public:
		virtual GfxResId CreateVertexBuffer(std::shared_ptr<GfxVertexBuffer>* OutBuffer) override;
		virtual GfxResId CreateIndexBuffer(std::shared_ptr<GfxIndexBuffer>* OutBuffer) override;
		virtual GfxResId CreateVertexArray(GfxVertexArray* OutBuffer) override;
	private:
		HWND HwnD;
		HDC hDC;
		HGLRC hRC;

		std::vector<std::shared_ptr<GLVertexBuffer>> VertexBuffers;
		std::vector<std::shared_ptr<GLIndexBuffer>> IndexBuffers;
	};
}
//#endif