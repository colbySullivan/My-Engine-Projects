#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>

//------------------------------------------------------------------------------
struct Rgba8;
struct Vertex;
class Camera;
class BitmapFont;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain; 
class Shader;
class VertexBuffer;
class ConstantBuffer;

//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool	m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
enum class BlendMode
{
	ALPHA,
	ADDITIVE,
#if defined(OPAQUE)
#undef OPAQUE
#endif
};

//------------------------------------------------------------------------------
#define DX_SAFE_RELEASE(dxObject)\
{\
    if ((dxObject) != nullptr)\
    {\
        (dxObject)->Release();\
        (dxObject) = nullptr;\
    }\
}

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

	void ClearScreen(Rgba8 const& clearColor);
	void BeginCamera(Camera const& camera);
	void EndCamera(Camera const& camera);
	void DrawVertexArray(int numVertexes, Vertex const* vertexes);
	void DrawVertexArray( std::vector<Vertex> const& verts );

	RenderConfig m_config;
	std::vector< Texture* > m_loadedTextures;
	std::vector< BitmapFont* > m_loadedFonts;
	
private:

	Shader* CreateShader( char const* shaderName, char const* shaderSource );
	Shader* CreateShader( char const* shaderName ); // File name way
	void BindShader( Shader* shader );
	bool CompileShaderToByteCode( std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target );

	// Constant Buffer
	ConstantBuffer* CreateConstantBuffer( const unsigned int size );
	void BindConstantBuffer( int slot, ConstantBuffer* cbo );
	void CopyCPUToGPU( const void* data, unsigned int size, ConstantBuffer* cbo );

	// Vertex Buffer
	VertexBuffer* CreateVertexBuffer( const unsigned int size, unsigned int stride );
	void BindVertexBuffer( VertexBuffer* vbo );
	void DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount );
	void CopyCPUToGPU( const void* data, unsigned int size, VertexBuffer* vbo );

protected:
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;
	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;

	std::vector<uint8_t> m_vertexShaderByteCode;
	std::vector<uint8_t> m_pixelShaderByteCode;

	ConstantBuffer* m_cameraCBO = nullptr;
};