#pragma once
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
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

	Shader* CreateShader(char const* shaderName, char const* shaderSource);
	bool CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name,
		char const* source, char const* entryPoint, char const* target);
	void BindShader(Shader* shader);


	VertexBuffer* CreateVertexBuffer(const unsigned int size, unsigned int stride);
	void CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo);
	void BindVertexBuffer(VertexBuffer* vbo);

	void DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount);
	
private:
protected:
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapChain = nullptr;

	VertexBuffer* m_immediateVBO = nullptr;
	std::vector<Shader*> m_loadedShaders;
	Shader* m_currentShader = nullptr;

	std::vector<uint8_t> m_vertexShaderByteCode;
	std::vector<uint8_t> m_pixelShaderByteCode;

	const char* defaultShaderSource = R"(
	struct vs_input_t
	{
		float3 localPosition : POSITION;
		float4 color : COLOR;
		float2 uv : TEXCOORD;
	};
	struct v2p_t
	{
		float4 position : SV_Position;
		float4 color : COLOR;
		float2 uv : TEXCOORD;
	};
	v2p_t VertexMain(vs_input_t input)
	{
		v2p_t v2p;
		v2p.position = float4(input.localPosition, 1);
		v2p.color = input.color;
		v2p.uv = input.uv;
		return v2p;
	};
	float4 PixelMain(v2p_t input) : SV_Target0
	{
		return float4(input.color);
	};
)";
};