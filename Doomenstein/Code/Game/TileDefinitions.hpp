#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <map>

class SpriteSheet;

//-----------------------------------------------------------------------------------------------
class TileDefinition
{
public:
	TileDefinition() = default;
	~TileDefinition() = default;

	static void InitializeTileDefs();
	static const TileDefinition* GetByName( const std::string& name );

public:
	std::string m_name;
	bool        m_isSolid = false;
	Rgba8       m_mapImagePixelColor;
	IntVec2     m_floorSpriteCoords;
	IntVec2     m_ceilingSpriteCoords;
	IntVec2     m_wallSpriteCoords;

	static std::map<std::string, TileDefinition> s_definitions;
};