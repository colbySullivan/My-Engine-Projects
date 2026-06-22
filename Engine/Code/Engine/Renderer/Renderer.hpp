#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>

#if defined(OPAQUE)
#undef OPAQUE
#endif

struct Rgba8;
struct Vertex;
class Camera;
class BitmapFont;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Model;
struct Mat44;

struct RenderConfig
{
	bool m_isEnabled = true;
};

enum class BlendMode
{
	OPAQUE,
	ALPHA,
	ADDITIVE,
	COUNT,
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_ONLY_LESS_EQUAL,
	READ_WRITE_LESS_EQUAL,
	COUNT
};

#define DX_SAFE_RELEASE(dxObject)\
{\
    if ((dxObject) != nullptr)\
    {\
        (dxObject)->Release();\
        (dxObject) = nullptr;\
    }\
}

class Renderer
{
public:
	Renderer( RenderConfig const& config );
	~Renderer();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void ClearScreen( Rgba8 const& clearColor );
	void BeginCamera( Camera const& camera );
	void EndCamera( Camera const& camera );
	void DrawVertexArray( int numVertexes, Vertex const* vertexes );
	void DrawVertexArray( std::vector<Vertex> const& verts );

	void SetBlendMode( BlendMode mode );
	void SetSamplerMode( SamplerMode mode );
	void SetRasterizerMode( RasterizerMode mode );
	void SetModelConstants( Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8( 255, 255, 255 ) );
	void SetDepthMode( DepthMode mode );

	Texture* CreateTextureFromImage( const Image& image );
	Texture* CreateTextureFromFile( const char* imageFilePath );
	Texture* CreateOrGetTextureFromFile( char const* imageFilePath );
	Texture* GetTextureForFileName( char const* imageFilePath );

	void BindTexture( Texture* texture, int textureRegisterSlot=0 );
	void BindSampler( SamplerMode mode, int samplerRegisterSlot=0 );

	BitmapFont* CreateOrGetBitmapFont( char const* bitmapFontFilePathWithNoExtension );
	BitmapFont* GetFontForFileName( char const* bitmapFontFilePathWithNoExtension );
	BitmapFont* CreateFontFromFile( char const* bitmapFontFilePathWithNoExtension );

	Model* CreateOrGetModelFromFile( char const* modelFilePath );
	Model* GetModelForFileName( char const* modelFilePath );
	Model* CreateModelFromFile( char const* modelFilePath );


	void CreateDepthStencilTexture();
	void CreateDepthStencilStates();
	void CreateDepthStencilState( DepthMode mode, D3D11_DEPTH_WRITE_MASK writeMask = D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_FUNC func = D3D11_COMPARISON_ALWAYS );

	Shader* CreateOrGetShader( const char* shaderName, VertexType vertexType = VertexType::VERTEX_PCU );

	void CopyCPUToGPU( const void* data, unsigned int size, ConstantBuffer* cbo );
	ConstantBuffer* CreateConstantBuffer( const unsigned int size );
	void BindConstantBuffer( int slot, ConstantBuffer* cbo );

	VertexBuffer* CreateVertexBuffer( const unsigned int size, unsigned int stride );
	void BindVertexBuffer( VertexBuffer* vbo );
	void DrawVertexBuffer( VertexBuffer* vbo, unsigned int vertexCount );
	void CopyCPUToGPU( const void* data, unsigned int size, VertexBuffer* vbo );

	void CopyCPUToGPU( const void* data, unsigned int size, IndexBuffer* ibo );
	IndexBuffer* CreateIndexBuffer( const unsigned int size );
	void DrawIndexBuffer( VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount );

	void BindShader( Shader* shader );

	ID3D11Device*        GetDevice()        const { return m_device; }
	ID3D11DeviceContext* GetDeviceContext()  const { return m_deviceContext; }

	RenderConfig m_config;
	std::vector<Texture*> m_loadedTextures;
	std::vector<BitmapFont*> m_loadedFonts;

	BlendMode m_desiredBlendMode;
	SamplerMode m_desiredSamplerMode;
	RasterizerMode m_desiredRasterizerMode;
	DepthMode m_desiredDepthMode;

	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	ID3D11DepthStencilView* m_depthStencilDSV = nullptr;
	ID3D11DepthStencilState* m_depthStencilStates[( int )( DepthMode::COUNT )] = {};
	ID3D11DepthStencilState* m_depthStencilState = nullptr;

protected:
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer* m_modelCBO = nullptr;
	IndexBuffer* m_indexVBO = nullptr;

	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;
	Shader* m_defaultShader = nullptr;

	std::vector<uint8_t> m_vertexShaderByteCode;
	std::vector<uint8_t> m_pixelShaderByteCode;

private:
	Shader* CreateShader( char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCU );
	Shader* CreateShader( char const* shaderName, char const* shaderSource, VertexType vertexType = VertexType::VERTEX_PCU );
	bool CompileShaderToByteCode( std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target );


	void SetStatesIfChanged();
	void CreateBlendStates( D3D11_BLEND sourceBlend, D3D11_BLEND destBlend, BlendMode mode );
	void CreateBlendAndSamplerStates();
	void CreateSamplerMode( SamplerMode mode, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressU, D3D11_TEXTURE_ADDRESS_MODE addressV, D3D11_TEXTURE_ADDRESS_MODE addressW );

	void CreateRasterizerState( D3D11_FILL_MODE fillMode, D3D11_CULL_MODE cullMode, RasterizerMode mode );
	void CreateNewRasterizerStates();

	void CreateBuffers();
	void CreateAndBindShaders();
	void CreateDeviceAndSwapChain();
	void DeleteReleaseAll();
	void EngineDubugRenderer();

	const Texture* m_defaultTexture = nullptr;
	Texture* m_currentTexture = nullptr;

	ID3D11BlendState* m_blendState = nullptr;
	ID3D11BlendState* m_blendStates[( int )( BlendMode::COUNT )] = {};

	ID3D11SamplerState* m_samplerState = nullptr;
	ID3D11SamplerState* m_samplerStates[( int )( SamplerMode::COUNT )] = {};

	ID3D11RasterizerState* m_rasterizerStates[( int )( RasterizerMode::COUNT )] = {};

	std::vector<Model*> m_loadedModels;
};