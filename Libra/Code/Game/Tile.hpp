#pragma once
#include "Game/TileDefinitions.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"

struct TileDefinitions;

class TileDef
{

public:
	static			void InitializeTileDefs();
	AABB2			m_UVs = AABB2(Vec2(0.f,0.f), Vec2(1.f,1.f));
	Rgba8			m_tint = Rgba8( 255, 255, 255, 255 );

	static	TileDef s_tileDefs[NUM_TILE_TYPE];

	bool	m_isSolid = false;
};

class Tile
{
public:
	Tile();
	~Tile();

	bool IsSolid() const;

public:
	IntVec2		m_tileCoords = IntVec2(2,1);
	TileTypes	m_type = TILE_TYPE_GRASS;

private:

};