#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>

#if defined(OPAQUE)
#undef OPAQUE
#endif

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
struct Mat44;

//-----------------------------------------------------------------------------------------------
struct RenderConfig
{
	bool	m_isEnabled = true;
};

//-----------------------------------------------------------------------------------------------
enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	COUNT,
};

//-----------------------------------------------------------------------------------------------
enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

//-----------------------------------------------------------------------------------------------
enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

//-----------------------------------------------------------------------------------------------
enum class DepthMode
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_ONLY_LESS_EQUAL,
	READ_WRITE_LESS_EQUAL,
	COUNT
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

	void SetBlendMode( BlendMode mode );
	void SetSamplerMode( SamplerMode mode );
	void SetRasterizerMode( RasterizerMode mode );
	Texture* CreateTextureFromImage( const Image& image );

	void SetModelConstants( Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8( 255, 255, 255 ) );

	RenderConfig m_config;
	std::vector< Texture* > m_loadedTextures;
	std::vector< BitmapFont* > m_loadedFonts;

	// Desired states
	BlendMode m_desiredBlendMode;
	SamplerMode m_desiredSamplerMode;
	RasterizerMode m_desiredRasterizerMode;
	DepthMode m_desiredDepthMode;


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

	// Blend mode
	void SetStatesIfChanged();
	void CreateBlendStates( D3D11_BLEND sourceBlend, D3D11_BLEND destBlend, BlendMode mode );
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11BlendState* m_blendStates[( int )( BlendMode::COUNT )] = {};

public:
	// Texture
	Texture* CreateTextureFromFile( const char* imageFilePath );
	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );
	Texture* GetTextureForFileName( char const* imageFilePath );
	BitmapFont* CreateOrGetBitmapFont( char const* bitmapFontFilePathWithNoExtension );
	BitmapFont* GetFontForFileName( char const* bitmapFontFilePathWithNoExtension );
	BitmapFont* CreateFontFromFile( char const* bitmapFontFilePathWithNoExtension );
	void BindTexture( Texture* texture );

	// Depth stencil texture
	void CreateDepthStencilTexture();
	void CreateDepthStencilStates();
	void CreateDepthStencilState( DepthMode mode, D3D11_DEPTH_WRITE_MASK writeMask = D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_FUNC func = D3D11_COMPARISON_ALWAYS);
	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	ID3D11DepthStencilView* m_depthStencilDSV = nullptr;
	ID3D11DepthStencilState* m_depthStencilStates[( int )( DepthMode::COUNT )] = {};
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

private:
	const Texture* m_defaultTexture = nullptr;
	Texture* m_currentTexture = nullptr;

	// Sampler
	ID3D11SamplerState* m_samplerState = nullptr;
	ID3D11SamplerState* m_samplerStates[( int )( SamplerMode::COUNT )] = {};

	// Rasterizer States
	ID3D11RasterizerState* m_rasterizerStates[( int )( RasterizerMode::COUNT )] = {};
	void CreateRasterizerState( D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, RasterizerMode mode );

	void CreateSamplerMode(  SamplerMode mode, D3D11_FILTER param1, D3D11_TEXTURE_ADDRESS_MODE param2, D3D11_TEXTURE_ADDRESS_MODE param3, D3D11_TEXTURE_ADDRESS_MODE param4 );
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
	ConstantBuffer* m_modelCBO = nullptr;
private:
	void CreateNewRasterizerStates();
	void CreateBlendAndSamplerStates();
	void CreateBuffers();
	void CreateAndBindShaders();
	void CreateDeviceAndSwapChain();
	void DeleteReleaseAll();
	void EngineDubugRenderer();
};