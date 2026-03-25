#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include <string>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

//------------------------------------------------------------------------------
struct ShaderConfig
{
	std::string m_name;
	std::string m_vertexEntryPoint = "VertexMain";
	std::string m_pixelEntryPoint = "PixelMain";
};

enum class VertexType
{
	VERTEX_PCU,
	VERTEX_PCUTBN,
	COUNT
};

//------------------------------------------------------------------------------
class Shader
{
	friend class Renderer;

public:
	Shader(const ShaderConfig& config);
	Shader(const Shader& copy) = delete;
	~Shader();

	const std::string& GetName() const;

	//Shader* CreateOrGetShader( const char* shaderName, VertexType vertexType = VertexType::VERTEX_PCU ); // #TODO Create or Get Shader In instructions Map Code Bottom last step too
	//Shader* CreateShader( char const* shaderName, VertexType vertexType = VertexType::VERTEX_PCU );
	//Shader* CreateShader( char const* shaderName, char const* shaderSource, VertexType vertexType = VertexType::VERTEX_PCU );

protected:
	ShaderConfig m_config;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};
