#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include "ThirdParty/stb/stb_image.h"
#define UNUSED(x) (void)(x);


#pragma comment(lib, "opengl32")

HGLRC g_openGLRenderingContext = nullptr; 

Renderer::Renderer( RenderConfig const& config )
	: m_config( config )
{

}

Renderer::~Renderer()
{

}

void Renderer::Startup()
{

}

void Renderer::Shutdown()
{

}

void Renderer::BeginFrame()
{
	
}

void Renderer::EndFrame()
{

}

void Renderer::ClearScreen(Rgba8 const& clearColor)
{

}

void Renderer::BeginCamera(Camera const& camera)
{
	Vec2 bottomLeft = camera.GetOrthoBottomLeft();
	Vec2 topRight = camera.GetOrthoTopRight();
	GLenum err;
}

void Renderer::EndCamera( [[maybe_unused]] Camera const& camera)
{

}

void Renderer::DrawVertexArray( std::vector<Vertex> const& verts )
{
	DrawVertexArray( static_cast< int >( verts.size() ), verts.data() );
}

void Renderer::DrawVertexArray(int numVertexes, Vertex const* vertexes)
{
	
}