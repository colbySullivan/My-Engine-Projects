#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <vector>

//------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;
class BitmapFont;

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
	RenderConfig m_config;
	std::vector< Texture* > m_loadedTextures;
	std::vector< BitmapFont* > m_loadedFonts;
	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );

private:
	Texture* GetTextureForFileName( char const* imageFilePath );
	Texture* CreateTextureFromFile( char const* imageFilePath );
	BitmapFont* CreateOrGetBitmapFont( char const* bitmapFontFilePathWithNoExtension ); // or std::string const&
	BitmapFont* GetFontForFileName( char const* bitmapFontFilePathWithNoExtension );
	BitmapFont* CreateFontFromFile( char const* bitmapFontFilePathWithNoExtension );
};