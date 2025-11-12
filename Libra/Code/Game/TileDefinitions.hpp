#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>

enum TileTypes
{
	INVALID_TILE_TYPE = -1,
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	TILE_TYPE_MUD,
	TILE_TYPE_PORTAL,
	TILE_TYPE_STONE_BARRIER,
	TILE_TYPE_STONE_FLOOR,
	TILE_TYPE_START,
	NUM_TILE_TYPES
};

class SpriteSheet;

class TileDefinition
{
public:
	static std::vector<TileDefinition> s_definitions;

	static void InitializeTileDefs();

	TileDefinition();
	TileDefinition( AABB2 uvs, Rgba8 tint, bool isSolid );
	~TileDefinition();

	AABB2 m_uvs = AABB2( Vec2( 0.f, 0.f ), Vec2( 1.f, 1.f ) );
	Rgba8 m_tint = Rgba8( 255, 255, 255, 255 );
	bool m_isSolid = false;
};