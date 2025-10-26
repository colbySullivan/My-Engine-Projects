#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <vector>

//------------------------------------------------------------------------------
enum TileType {
	GRASS,
	STONE
};

//------------------------------------------------------------------------------
class Tile {
public:
	IntVec2 m_tileCoords;
	TileType m_type;
};

//------------------------------------------------------------------------------
class Map
{
	RandomNumberGenerator g_rng;

	std::vector< Tile > m_tiles; // Note: this is NOT a 2D array!
	IntVec2 m_dimensions; // # of tiles wide (x) and high (y)

public:	
	Map(IntVec2 dimensions);

	// Tile access
	int GetTileIndexForTileCoords(IntVec2 tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int tileCoords) const;

	// Rendering
	void Render() const;
	void BuildMapTiles();
	void BarrierTileSetup();
	void OutEdgeStoneSetup();
	void StoneTileSetup();
	void GrassTileSetup();
};