#pragma once
#include "Engine/Core/Texture.hpp"
#include <vector>

//------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;

//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool	m_isEnabled = true;
};

//------------------------------------------------------------------------------
class Renderer
{
public:
	Renderer( RenderConfig const& config );
	~Renderer();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void CreateRenderingContext();
	void ClearScreen(Rgba8 const& clearColor);
	void BeginCamera(Camera const& camera);
	void EndCamera(Camera const& camera);
	void DrawVertexArray(int numVertexes, Vertex const* vertexes);

	void DrawVertexArray( std::vector<Vertex> const& verts );
	Texture* CreateTextureFromData( char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData );
	void BindTexture( Texture* texture );
	Texture* CreateTextureFromFile( char const* imageFilePath );
	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );
	Texture* GetTextureForFileName( char const* imageFilePath );
	RenderConfig m_config;
	std::vector< Texture* > m_loadedTextures;
};