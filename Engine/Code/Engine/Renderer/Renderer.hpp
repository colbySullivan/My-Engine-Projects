#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <vector>
#include <d3d11.h>

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
	
private:
protected:
	//ID3D11VertexShader* m_vertexShader = nullptr;
	//ID3D11PixelShader* m_pixelShader = nullptr;
	//ID3D11InputLayout* m_inputLayoutForVertex_PCU = nullptr;
	//ID3D11Buffer* m_vertexBuffer = nullptr;

	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;
};