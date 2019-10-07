#include "GfxGLGDI.h"
#include "inc_core.h"
#include "GLShaders.h"
#include "GLTexture.h"
#include "GLImmediatePrimitives.h"
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

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(HwnD, hDC);

		DWN_CORE_INFO("OpenGL GDI shutdown!");
	}

	void GfxGLGDI::DrawIndexed(GfxResId VertexArrayId)
	{
		GLVertexArray* vertexArray = (GLVertexArray*)this->GetVertexArray(VertexArrayId);
		if (vertexArray)
		{
			vertexArray->Bind();
			glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer(this)->GetSize(), GL_UNSIGNED_INT, 0);
			vertexArray->Unbind();
		}
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

	/*void GfxGLGDI::DrawLine(u32 First, u32 Count)
	{
		glDrawArrays(GL_LINE_STRIP, First, Count);
	}*/


	GfxResId GfxGLGDI::CreateVertexBuffer(float* Vertices, u32 Size, GfxVertexBuffer** OutBuffer)
	{
		VertexBufferBundle* Bundle = VertexBufferPool.Request();
		Bundle->Element =new GLVertexBuffer(Vertices, Size, Bundle->GetId());

		if (OutBuffer != nullptr)
			*OutBuffer = Bundle->Element;

		return Bundle->GetId();
	}


	GfxResId GfxGLGDI::CreateIndexBuffer(u32* Indices, u32 Size, GfxIndexBuffer** OutBuffer)
	{
		IndexBufferBundle* Bundle = IndexBufferPool.Request();
		Bundle->Element = new GLIndexBuffer(Indices, Size, Bundle->GetId());

		if (OutBuffer != nullptr)
			*OutBuffer = Bundle->Element;

		return Bundle->GetId();
	}

	GfxResId GfxGLGDI::CreateVertexArray(GfxVertexArray** OutBuffer)
	{
		VertexArrayBundle* Bundle = VertexArrayPool.Request();
		Bundle->Element = new GLVertexArray(Bundle->GetId());
		
		if (OutBuffer != nullptr)
			*OutBuffer = Bundle->Element;

		return Bundle->GetId();
	}

	GfxResId GfxGLGDI::CreateShader(GfxShader** OutShader)
	{
		ShaderBundle* Bundle = ShaderPool.Request();
		Bundle->Element = new GLShader(Bundle->GetId());

		if (OutShader != nullptr)
			*OutShader = Bundle->Element;
		
		return Bundle->GetId();
	}

	GfxResId GfxGLGDI::CreateTexture(u8* Data, u32 Width, u32 Height, u16 ChannelsPerPixel, GfxWrapDesc Wrap,
		GfxFilterDesc Filter, bool GenerateMipMaps, GfxTexture** OutTexture)
	{
		TextureBundle* Bundle = TexturePool.Request();
		Bundle->Element = new GLTexture(Bundle->GetId(), Data, Width, Height, ChannelsPerPixel, Wrap, Filter, GenerateMipMaps);

		if(OutTexture != nullptr)
			*OutTexture = Bundle->Element;

		return Bundle->GetId();
	}

}
//#endif