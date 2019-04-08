#include "GfxOpenglGDI.h"
#include "inc_core.h"
//#ifdef USE_OPENGL_GFX

namespace Dawn
{
	bool GfxOpenglGDI::Init(const AppSettings& InSettings)
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

		DWN_CORE_INFO("OpenGL ({0}.{1}) GDI initialized!", GLVersion.major, GLVersion.minor);

		return true;
	}

	void GfxOpenglGDI::Present()
	{
		SwapBuffers(hDC);
	}

	void GfxOpenglGDI::Shutdown()
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hRC);
		ReleaseDC(HwnD, hDC);

		DWN_CORE_INFO("OpenGL GDI shutdown!");
	}
}
//#endif