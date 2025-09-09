#include <windows.h>
#include "Renderer.hpp";
#include "Engine/Core/Vertex.hpp";
#include <gl/GL.h>
#pragma comment(lib, "opengl32")

HGLRC g_openGLRenderingContext = nullptr; 

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

void Renderer::Startup()
{
	CreateRenderingContext();
}

void Renderer::Shutdown()
{

}

void Renderer::BeginFrame()
{
	// Todo Need to move this back into Begin camera
	glLoadIdentity();
	glOrtho(0.f, 200.f, 0.f, 100.f, 0.f, 1.f); // arguments are: xLeft, xRight, yBottom, yTop, zNear, zFar

	ClearScreen(Rgba8(0.39215686274f, 0.19607843137f, 0.f, 1.f));

	glBegin(GL_TRIANGLES);
}

void Renderer::EndFrame()
{
	glEnd();
}

void Renderer::CreateRenderingContext()
{
	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
	pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	HWND windowHandle = ::GetActiveWindow(); // #TempHack  Later, the Window subsystem will hold this
	HDC displayDeviceContext = GetDC(windowHandle);

	// These two OpenGL-like functions (wglCreateContext and wglMakeCurrent) will remain here for now.
	int pixelFormatCode = ChoosePixelFormat(displayDeviceContext, &pixelFormatDescriptor);
	SetPixelFormat(displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
	g_openGLRenderingContext = wglCreateContext(displayDeviceContext);
	wglMakeCurrent(displayDeviceContext, g_openGLRenderingContext);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{
	// Todo clearColor not working
	glClearColor(0.39215686274f, 0.19607843137f, 0.f, 1.f); // Note; glClearColor takes colors as floats in [0,1], not bytes in [0,255]
	glClear(GL_COLOR_BUFFER_BIT); // ALWAYS clear the screen at the top of each frame's Render()!	
}

void Renderer::BeginCamera(Camera const& camera)
{
	glLoadIdentity();
	glOrtho(0.f, 200.f, 0.f, 100.f, 0.f, 1.f); // arguments are: xLeft, xRight, yBottom, yTop, zNear, zFar
}

void Renderer::EndCamera(Camera const& camera)
{

}

void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	for (int i = 0; i < numVertexes; ++i)
		{
		Vertex const& vert = vertexes[i];
		glColor4ub(vert.m_color.r, vert.m_color.g, vert.m_color.b, vert.m_color.a);
		glTexCoord2f(vert.m_uvTexCoords.x, vert.m_uvTexCoords.y);
		glVertex3f(vert.m_position.x, vert.m_position.y, vert.m_position.z);
		}
}

