#pragma once
#include "Engine/Math/AABB2.hpp"
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
	TileDefinition( AABB2 uvs, Rgba8 tint, bool isSolid );
	~TileDefinition();

	AABB2 m_uvs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	Rgba8 m_tint = Rgba8( 255, 255, 255, 255 );
	bool m_isSolid = false;
};