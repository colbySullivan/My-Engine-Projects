#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include <vector>

class Entity;
//------------------------------------------------------------------------------
enum TileType {
	GRASS,
	STONE
};

//------------------------------------------------------------------------------
class Tile {
public:
	IntVec2 m_tileCoords;
	TileType m_type = GRASS;
};

//------------------------------------------------------------------------------
class Map
{
	RandomNumberGenerator g_rng;

	std::vector< Tile > m_tiles; // Note: this is NOT a 2D array!
	std::vector< Entity* > m_entities;
	IntVec2 m_dimensions; // # of tiles wide (x) and high (y)

public:	
	Map( Game* game, IntVec2 dimensions );

	// Tile access
	int GetTileIndexForTileCoords(IntVec2 tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int tileCoords) const;
	bool IsTileSolidAtTileCoords(IntVec2 tileCoords) const;
	void PushEntityOutOfTileIfSolid( Entity& e, IntVec2 const& tileCoords );
	void PushEntityOutOfWalls( Entity& e, float deltaSeconds );
	AABB2 GetTileBounds( IntVec2 const& tileCoords ) const;
	IntVec2 GetTileCoordsForWorldPos(Vec2 const& worldPos) const;

	// Rendering
	void Render() const;
	void BuildMapTiles();
	void BarrierTileSetup();
	void OutEdgeStoneSetup();
	void StoneTileSetup();
	void GrassTileSetup();

	// Utility
	void Update( float deltaSeconds );
	void AddToEntityVector( Entity* e );


private:
	Game* m_game = nullptr;
	void UpdateEntities( float deltaSeconds );
};