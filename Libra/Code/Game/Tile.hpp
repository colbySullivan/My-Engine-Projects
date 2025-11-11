#pragma once
#include "Game/TileDefinitions.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

struct TileDefinitions;

class Tile
{
public:
	Tile();
	Tile( IntVec2 tileCoords, TileTypes type );
	~Tile();

	bool IsSolid() const;
	TileDefinition const& GetDefinition() const;

	IntVec2 m_tileCoords = IntVec2( -1, -1 );
	TileTypes m_type = NUM_TILE_TYPES;
};