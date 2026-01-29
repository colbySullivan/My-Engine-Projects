#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
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
class IDXGISwapChain; 
class Shader;

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

	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;

	Shader* CreateShader(char const* shaderName, char const* shaderSource);
	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
		char const* source, char const* entryPoint, char const* target);
	void BindShader(Shader* shader);
	
private:
protected:
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
};