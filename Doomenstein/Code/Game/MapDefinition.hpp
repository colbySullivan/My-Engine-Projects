#pragma once
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <string>
#include <map>

//-----------------------------------------------------------------------------------------------
struct SpawnInfo
{
	std::string		m_name;
	Vec3			m_spawnLocation;
	EulerAngles		m_actorOrientation;
};

class MapDefinition
{
public:
	MapDefinition() = default;
	static void InitializeMapDefs();
	static const MapDefinition* GetByName( const std::string& name );
	static MapDefinition* CreateMapDef( std::string name, std::string imagePath, std::string shaderPath, std::string texturePath, IntVec2 cellCount );

	static std::map<std::string, MapDefinition*> s_definitions;
	std::string				m_name;
	Image					m_image;
	Shader*					m_shader;
	Texture*				m_spriteSheetTexture;
	IntVec2					m_spriteSheetCellCount;
	std::vector<SpawnInfo>	m_spawnInfos;
};