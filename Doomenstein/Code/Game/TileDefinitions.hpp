#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>
#include <string>
#include <map>

class SpriteSheet;

class TileDefinition
{
public:
	static std::map<std::string, TileDefinition> s_definitions;

	static void InitializeTileDefs();

	TileDefinition();
	~TileDefinition();

	std::string m_name;
	bool m_isSolid = false;
	Rgba8 m_mapImagePixelColor;
	IntVec2 m_floorSpriteCoords;
	IntVec2 m_ceilingSpriteCoords;
	IntVec2 m_wallSpriteCoords;
};