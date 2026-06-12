	#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/HeatMaps.hpp"
#include "Game/Game.hpp"
#include "Game/TileDefinitions.hpp"
#include "Game/Tile.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct MapDef
{
	IntVec2		m_dimensions;
	std::string	m_fillTileType		= "INVALID_TILE_TYPE";
	std::string	m_edgeTileType		= "INVALID_TILE_TYPE";
	float		m_hordeTimer;
	float		m_bigBoyTimer;
	float		m_difficultyMultiplier;
};

//-----------------------------------------------------------------------------------------------
class Entity;

//------------------------------------------------------------------------------
class Map
{
	RandomNumberGenerator g_rng;
public:
	std::vector< Tile > m_tiles; // Note: this is NOT a 2D array!
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
	void RenderHeatMapTiles() const;
	void RenderSolidMapTiles() const;
	void RenderEntities() const;
	void UpdateCameras();
	void BuildMapTiles();
	void OutEdgeTileSetup();
	void FillTile1Setup();

	// Update
	void Update( float deltaSeconds );
	void UpdatePlayerDevControls( XboxController const& controller );

	// Utility
	bool HasLineOfSight( Vec2 posA, Vec2 posB) const;
	RaycastResult2D RaycastVsTiles( Vec2 startPos, Vec2 fwdNormal, float maxDist ) const;
	Entity* SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, EntityFaction faction );
	void AddEntityToMap( Entity& e );
	void RemoveEntityFromMap( Entity& e );
	void DestroyGarbageEntities();
	bool IsPlayerAlive();
	void CreateInitialEntities();
	void PopulateDijkstraMap( TileHeatMap& out_dijkstraMap, IntVec2 startCoords, float maxCost, bool treatWaterAsSolid=true );

	bool m_lostGame = false;

	EntityList m_entityListsByType[NUM_ENTITY_TYPES];
	EntityList m_allEntities;
	MapDef m_mapDef;

	//TileHeatMap		m_distMapFromStartToPlayer;
	//TileHeatMap		m_visibilityMap;


private:
	Game* m_game = nullptr;
	void UpdateEntities( float deltaSeconds );
	Vec2 GetRandomValidPointInMapVec2();
	IntVec2 GetRandomValidPointInMapIntVec2();
	TileHeatMap* m_heatMap;
	void SwtichMapRenderMode() const;
	void UpdateHandlePlayerHitCollisions();

private:
	void UpdateSpawners();
	void SpawnSmallHorde();
	void SpawnBigBoy();
	Timer* m_hordeTimer = nullptr;
	Timer* m_bigBoyTimer = nullptr;
	EntityType m_hordeType;
	EntityType m_bigBoyType;

};