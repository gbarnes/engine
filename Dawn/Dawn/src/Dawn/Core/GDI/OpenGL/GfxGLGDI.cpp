#include "GfxGLGDI.h"
#include "inc_core.h"
#include "GLShaders.h"
#include "GLTexture.h"
#include "GLImmediatePrimitives.h"
#include "GLRenderBuffer.h"
#include "Core/Memory/MemoryUtils.h"
#include "../GfxRenderDoc.h"
//#ifdef USE_OPENGL_GFX

namespace Dawn
{
	void 
		MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam)
	{
		if(type == GL_DEBUG_TYPE_ERROR)
			DWN_CORE_ERROR(message);
		else 
			DWN_CORE_INFO(message);
	}

	bool GfxGLGDI::Init(const AppSettings& InSettings)
	{
		PIXELFORMATDESCRIPTOR pfd;
		int format;
		HwnD = InSettings.Hwnd;

		hDC = GetDC(HwnD);

		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = InSettings.ColorBits;
		pfd.cDepthBits = InSettings.DepthBits;
		pfd.iLayerType = PFD_MAIN_PLANE;

		format = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, format, &pfd);

		HGLRC tempOpenGLContext = wglCreateContext(hDC); // Create an OpenGL 2.1 context for our device context
		wglMakeCurrent(hDC, tempOpenGLContext); // Make the OpenGL 2.1 context current and active

		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4, // Set the MAJOR version of OpenGL to 3
			WGL_CONTEXT_MINOR_VERSION_ARB, 6, // Set the MINOR version of OpenGL to 2
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
			0
		};

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB) {
			hRC = wglCreateContextAttribsARB(hDC, NULL, attributes); // Create and OpenGL 3.x context based on the given attributes
			wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
			wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context
			wglMakeCurrent(hDC, hRC); // Make our OpenGL 3.2 context current

			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			wglSwapIntervalEXT((InSettings.UseVsync) ?  1 : 0); //VSYNC NOT LOCKED TO 60
		}
		else {
			hRC = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context
			printf("No OpenGL 3.2 Support, fallback Version 2.1");
		}
		
		if (!gladLoadGL())
		{
			DWN_CORE_ERROR("Failed to initialize GLAD");
			return false;
		}

		if (GLAD_GL_VERSION_3_0) {
			/* We support at least OpenGL version 3 */
		}

		if (InSettings.ShowGDIDebug)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);
		}

		DWN_CORE_INFO("OpenGL ({0}.{1}) GDI initialized!", GLVersion.major, GLVersion.minor);
		
		glEnable(GL_DEPTH_TEST);

		Primitives = std::make_unique<GLImmediatePrimitives>(GLImmediatePrimitives(this->shared_from_this()));

		RenderDoc::StartFrameCapture();

		return true;
	}

	void GfxGLGDI::Present()
	{
		SwapBuffers(hDC);
	}

	void GfxGLGDI::Shutdown()
	{
		this->VertexArrayPool.Clear();
		this->VertexBufferPool.Clear();
		this->IndexBufferPool.Clear();
		this->ShaderPool.Clear();
		this->RenderBufferPool.Clear();
		
		RenderDoc::EndFrameCapture();

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(HwnD, hDC);

		DWN_CORE_INFO("OpenGL GDI shutdown!");
	}

	void GfxGLGDI::DrawIndexed(GfxResId VertexArrayId)
	{
		GLVertexArray* vertexArray = (GLVertexArray*)this->GetVertexArray(VertexArrayId);
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer(this)->GetSize(), GL_UNSIGNED_INT, 0);
		vertexArray->Unbind();
	}

	void GfxGLGDI::DrawArray(GfxResId VertexArrayId)
	{
		GLVertexArray* vertexArray = (GLVertexArray*)this->GetVertexArray(VertexArrayId);
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffer(this, 0)->GetSize());
		vertexArray->Unbind();
	}

	void GfxGLGDI::DrawInstanced(GfxResId VertexArrayId, u32 InAmount)
	{
		GLVertexArray* vertexArray = (GLVertexArray*)this->GetVertexArray(VertexArrayId);
		vertexArray->Bind();
		u32 Size = vertexArray->GetIndexBuffer(this)->GetSize();
		glDrawElementsInstanced(GL_TRIANGLES, Size, GL_UNSIGNED_INT, 0, InAmount);
		vertexArray->Unbind();
	}

	void GfxGLGDI::Clear()
	{
		glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GfxGLGDI::SetViewport(u32 InLeft, u32 InTop, u32 InRight, u32 InBottom)
	{
		glViewport(InLeft, InTop, InRight, InBottom);
	}

	void GfxGLGDI::ActivateTextureSlot(u32 InIndex)
	{
		glActiveTexture(GL_TEXTURE0 + InIndex);
	}

	GfxResId GfxGLGDI::CreateVertexBuffer(void* Vertices, u32 Size, GfxVertexBuffer** OutBuffer)
	{
		VertexBufferSlot* Slot = VertexBufferPool.Request();
		Allocators::LinearAllocator* Arena = VertexBufferPool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLVertexBuffer), __alignof(GLVertexBuffer), 0)) GLVertexBuffer(Vertices, Size, Slot->GetId()); //D_NEW_ALLOC(GLVertexBuffer, 0, Arena)(Vertices, Size, Slot->GetId());

		if (OutBuffer != nullptr)
			*OutBuffer = Slot->Element;

		return Slot->GetId();
	}


	GfxResId GfxGLGDI::CreateIndexBuffer(u32* Indices, u32 Size, GfxIndexBuffer** OutBuffer)
	{
		IndexBufferSlot* Slot = IndexBufferPool.Request();
		Allocators::LinearAllocator* Arena = IndexBufferPool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLIndexBuffer), __alignof(GLIndexBuffer), 0)) GLIndexBuffer(Indices, Size, Slot->GetId()); //D_NEW_ALLOC(GLIndexBuffer, 0, Arena)(Indices, Size, Slot->GetId());

		if (OutBuffer != nullptr)
			*OutBuffer = Slot->Element;

		return Slot->GetId();
	}

	GfxResId GfxGLGDI::CreateVertexArray(GfxVertexArray** OutBuffer)
	{
		VertexArraySlot* Slot = VertexArrayPool.Request();
		Allocators::LinearAllocator* Arena = VertexArrayPool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLVertexArray), __alignof(GLVertexArray), 0)) GLVertexArray(Slot->GetId()); //D_NEW_ALLOC(GLVertexArray, 0, Arena)(Slot->GetId());
		
		if (OutBuffer != nullptr)
			*OutBuffer = Slot->Element;

		return Slot->GetId();
	}

	GfxResId GfxGLGDI::CreateShader(GfxShader** OutShader)
	{
		ShaderSlot* Slot = ShaderPool.Request();
		Allocators::LinearAllocator* Arena = ShaderPool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLShader), __alignof(GLShader), 0)) GLShader(Slot->GetId());//D_NEW_ALLOC(GLShader, 0, Arena)(Slot->GetId());

		if (OutShader != nullptr)
			*OutShader = Slot->Element;
		
		return Slot->GetId();
	}

	GfxResId GfxGLGDI::CreateTexture(u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
		GfxFilterDesc Filter, bool GenerateMipMaps, GfxTexture** OutTexture)
	{
		TextureSlot* Slot = TexturePool.Request();
		Allocators::LinearAllocator* Arena = TexturePool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLTexture), __alignof(GLTexture), 0)) GLTexture(Slot->GetId(), Data, Width, Height, ChannelsPerPixel, Wrap, Filter, GenerateMipMaps);//D_NEW_ALLOC(GLTexture, 0, Arena)(Slot->GetId(), Data, Width, Height, ChannelsPerPixel, Wrap, Filter, GenerateMipMaps);

		if(OutTexture != nullptr)
			*OutTexture = Slot->Element;

		return Slot->GetId();
	}

	GfxResId GfxGLGDI::CreateRenderBuffer(GfxRenderBuffer** OutTexture)
	{
		RenderBufferSlot* Slot = RenderBufferPool.Request();
		Allocators::LinearAllocator* Arena = RenderBufferPool.GetMemArena();
		Slot->Element = new (Arena->Allocate(sizeof(GLRenderBuffer), __alignof(GLRenderBuffer), 0)) GLRenderBuffer(Slot->GetId());//D_NEW_ALLOC(GLRenderBuffer, 0, Arena) (Slot->GetId());

		if (OutTexture != nullptr)
			*OutTexture = Slot->Element;

		return Slot->GetId();
	}

}
//#endif