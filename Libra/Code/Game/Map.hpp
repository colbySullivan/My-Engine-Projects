#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinitions.hpp"
#include "Game/Tile.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct MapDef
{
	IntVec2		m_dimensions;
	TileTypes	m_fillTileType		= INVALID_TILE_TYPE;
	TileTypes	m_edgeTileType		= INVALID_TILE_TYPE;
	TileTypes	m_barrierTileType	= INVALID_TILE_TYPE;
	TileTypes	m_sprinkle1TileType = INVALID_TILE_TYPE;
	TileTypes	m_sprinkle2TileType = INVALID_TILE_TYPE;
};

//-----------------------------------------------------------------------------------------------
class Entity;

//------------------------------------------------------------------------------
class Map
{
	RandomNumberGenerator g_rng;

	std::vector< Tile > m_tiles; // Note: this is NOT a 2D array!
	//std::vector< Entity* > m_entities;
	IntVec2 m_dimensions; // # of tiles wide (x) and high (y)
	int m_numTilesInViewVertically;
	bool m_debugCamera;

public:	
	//Map( Game* game, IntVec2 dimensions );
	Map( Game* game, MapDef const& mapDefinition );

	// Tile access
	int GetTileIndexForTileCoords(IntVec2 tileCoords) const;
	IntVec2 GetTileCoordsForIndex(int tileCoords) const;
	bool IsTileSolidAtTileCoords(IntVec2 tileCoords) const;
	void PushEntityOutOfEachOther() const;
	void PushEntityOutOfTileIfSolid( Entity& e, IntVec2 const& tileCoords );
	void PushEntityOutOfWalls( Entity& e, float deltaSeconds );
	AABB2 GetTileBounds( IntVec2 const& tileCoords ) const;
	IntVec2 GetTileCoordsForWorldPos(Vec2 const& worldPos) const;

	// Rendering
	void Render() const;
	void RenderTiles() const;
	void RenderEntities() const;
	void UpdateCameras();
	void BuildMapTiles();
	void SpawnBarrierTileSetup();
	void OutEdgeTileSetup();
	void SprinkleTileSetup();
	void FillTile1Setup();

	// Update
	void Update( float deltaSeconds );
	void UpdatePlayerDevControls( XboxController const& controller );

	// Utility
	void AddToEntityVector( Entity* e );
	bool HasLineOfSight( Vec2 posA, Vec2 posB) const;
	RaycastResult2D RaycastVsTiles( Vec2 startPos, Vec2 fwdNormal, float maxDist ) const;
	Entity* SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, EntityFaction faction );
	void AddEntityToMap( Entity& e );
	void RemoveEntityFromMap( Entity& e );
	void DestroyGarbageEntities();
	bool IsPlayerOnPortal();
	bool IsPlayerAlive();
	void CreateInitialEntities();

	bool m_lostGame = false;

	EntityList m_entityListsByType[NUM_ENTITY_TYPES];
	EntityList m_allEntities;


private:
	Game* m_game = nullptr;
	void UpdateEntities( float deltaSeconds );
	Vec2 GetRandomValidPointInMap();
	void CheckLineOfSights();
	TileTypes m_fillTileType;
	TileTypes m_sprinkle1TileType;
	TileTypes m_sprinkle2TileType;
	TileTypes m_edgeTileType;
	TileTypes m_barrierTileType;
};