#pragma once
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <string>
#include <map>

class MapDefinition
{
public:
	MapDefinition() = default;
	static void InitializeMapDefs();
	static const MapDefinition* GetByName( const std::string& name );
	static MapDefinition* CreateMapDef( std::string name, std::string imagePath, std::string shaderPath, std::string texturePath, IntVec2 cellCount );

	static std::map<std::string, MapDefinition*> s_definitions;
	std::string		m_name;
	Image			m_image;
	Shader* m_shader;
	Texture* m_spriteSheetTexture;
	IntVec2			m_spriteSheetCellCount;
};