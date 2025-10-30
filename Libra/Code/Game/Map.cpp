#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------
Map::Map( Game* game, IntVec2 dimensions )
	: m_game( game )
	, m_dimensions( dimensions )
{
	m_numTilesInViewVertically = 10;
	BuildMapTiles();
	AddToEntityVector(new Player(m_game, Vec2(2.f, 2.f)));
}

//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds)
{
	UpdateEntities( deltaSeconds );
}

//------------------------------------------------------------------------------
int Map::GetTileIndexForTileCoords(IntVec2 tileCoords) const
{
	return (tileCoords.y * m_dimensions.x) + tileCoords.x;
}

//------------------------------------------------------------------------------
IntVec2 Map::GetTileCoordsForIndex(int tileCoords) const
{
	int y = tileCoords / m_dimensions.x;
	int x = tileCoords % m_dimensions.x;
	return IntVec2(x, y);
}

//------------------------------------------------------------------------------
bool Map::IsTileSolidAtTileCoords(IntVec2 tileCoords) const
{
	return m_tiles[GetTileIndexForTileCoords(tileCoords)].m_type != GRASS;
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfTileIfSolid( Entity& e, IntVec2 const& tileCoords )
{
	if ( !IsTileSolidAtTileCoords( tileCoords ) )
	{
		return;
	}

	AABB2 tileBounds = GetTileBounds( tileCoords );
	Vec2 nearestPointOnTile = tileBounds.GetNearestPoint( e.m_position );
	PushDiscOutOfFixedPoint2D( e.m_position, e.m_physicsRadius, nearestPointOnTile );
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfWalls( Entity& e, [[maybe_unused]] float deltaSeconds )
{
	if ( e.m_noClip && e.IsPlayer() )
	{
		return;
	}

	IntVec2 myTileCoords = GetTileCoordsForWorldPos( e.m_position );


	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_EAST );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_WEST );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NORTH );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SOUTH );

	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NE );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_NW );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SE );
	PushEntityOutOfTileIfSolid( e, myTileCoords + STEP_SW );
}

//------------------------------------------------------------------------------
AABB2 Map::GetTileBounds(IntVec2 const& tileCoords) const
{
	float minX = static_cast<float>(tileCoords.x);
	float minY = static_cast<float>(tileCoords.y);
	float maxX = minX + 1.0f;
	float maxY = minY + 1.0f;

	return AABB2(minX, minY, maxX, maxY);
}

IntVec2 Map::GetTileCoordsForWorldPos( Vec2 const& worldPos ) const
{
	int tileX = static_cast< int >( floorf( worldPos.x ) );
	int tileY = static_cast< int >( floorf( worldPos.y ) );
	return IntVec2( tileX, tileY );
}
//-----------------------------------------------------------------------------------------------
// Render
//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	RenderUpdateCameras();
	RenderTiles();
	RenderEntities();
}

//-----------------------------------------------------------------------------------------------
void Map::RenderUpdateCameras() const
{
	Player* player = nullptr;
	for ( int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[i] && m_entities[i]->IsPlayer() )
		{
			player = ( Player* )m_entities[i];
			break;
		}
	}

	if ( player )
	{
		Vec2 cameraCenter = player->m_position;
		float halfHeight = m_numTilesInViewVertically * 0.5f;
		float halfWidth = halfHeight * 2.0f;
		Vec2 cameraMin = cameraCenter - Vec2( halfWidth, halfHeight );
		Vec2 cameraMax = cameraCenter + Vec2( halfWidth, halfHeight );

		float cameraWidth = cameraMax.x - cameraMin.x;
		float cameraHeight = cameraMax.y - cameraMin.y;

		cameraMin.x = GetClamped( cameraMin.x, 0.f, static_cast< float >( m_dimensions.x ) - cameraWidth );
		cameraMax.x = cameraMin.x + cameraWidth;

		cameraMin.y = GetClamped( cameraMin.y, 0.f, static_cast< float >( m_dimensions.y ) - cameraHeight );
		cameraMax.y = cameraMin.y + cameraHeight;

		m_game->m_worldCamera->SetOrthoView( cameraMin, cameraMax );
	}
	
}

//-----------------------------------------------------------------------------------------------
void Map::RenderTiles() const
{
	std::vector<Vertex> tileVerts;

	int totalTiles = static_cast< int >( m_tiles.size() );
	tileVerts.reserve( totalTiles * 6 );

	for ( int tileIndex = 0; tileIndex < totalTiles; ++tileIndex )
	{
		Tile const& tile = m_tiles[tileIndex];

		float minX = static_cast< float >( tile.m_tileCoords.x );
		float minY = static_cast< float >( tile.m_tileCoords.y );
		float maxX = minX + 1.0f;
		float maxY = minY + 1.0f;

		AABB2 box = AABB2( minX, minY, maxX, maxY );
		Rgba8 tileColor;
		if ( tile.m_type == GRASS )
		{
			tileColor = GRASS_COLOR;
		}
		else
		{
			tileColor = STONE_COLOR;
		}

		AddVertsForAABB2D( tileVerts, box, tileColor );
	}
	TransformVertexArrayXY3D( static_cast< int >( tileVerts.size() ), tileVerts.data(), 1.0f, 0.0f, Vec2( 0.0f, 0.0f ) );
	g_engine->m_render->DrawVertexArray( static_cast< int >( tileVerts.size() ), tileVerts.data() );
}

//-----------------------------------------------------------------------------------------------
void Map:: RenderEntities() const
{
	for ( int entityIndex = 0; entityIndex < m_entities.size(); entityIndex++ )
	{
		if ( m_entities[entityIndex] )
		{
			m_entities[entityIndex]->Render();
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::BuildMapTiles()
{
	int totalTiles = m_dimensions.x * m_dimensions.y;
	m_tiles.resize(totalTiles);

	GrassTileSetup();
	StoneTileSetup();
	OutEdgeStoneSetup();
	BarrierTileSetup();
}




//------------------------------------------------------------------------------
// Tile Setups
//------------------------------------------------------------------------------
void Map::BarrierTileSetup()
{
	m_tiles[GetTileIndexForTileCoords(IntVec2(2, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(3, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 4))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 3))].m_type = STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 2))].m_type = STONE;
}

//------------------------------------------------------------------------------
void Map::OutEdgeStoneSetup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			bool isEdge = (tileX == 0 || tileX == m_dimensions.x - 1 ||
				tileY == 0 || tileY == m_dimensions.y - 1);

			if (isEdge)
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = STONE;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::StoneTileSetup()
{
	for (int tileY = 1; tileY < m_dimensions.y - 1; ++tileY)
	{
		for (int tileX = 1; tileX < m_dimensions.x - 1; ++tileX)
		{

			if (g_rng.RollRandomFloatZeroToOne() < 0.1f && (tileX > 5 || tileY > 5))
			{
				int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
				m_tiles[tileIndex].m_type = STONE;
			}
		}
	}
}

//------------------------------------------------------------------------------
void Map::GrassTileSetup()
{
	for (int tileY = 0; tileY < m_dimensions.y; ++tileY)
	{
		for (int tileX = 0; tileX < m_dimensions.x; ++tileX)
		{
			int tileIndex = GetTileIndexForTileCoords(IntVec2(tileX, tileY));
			m_tiles[tileIndex].m_tileCoords = IntVec2(tileX, tileY);
			m_tiles[tileIndex].m_type = GRASS;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::AddToEntityVector( Entity* e )
{
	m_entities.reserve( static_cast< int >( m_entities.size() ) + 1 );
	m_entities.push_back( e );
}

//-----------------------------------------------------------------------------------------------
void Map::UpdateEntities( float deltaSeconds )
{
	for (int entityIndex = 0; entityIndex < m_entities.size() ; entityIndex++)
	{
		Entity* entityAtIndex = m_entities[entityIndex];
		if ( entityAtIndex )
		{
			entityAtIndex->Update( deltaSeconds );
			PushEntityOutOfWalls( *entityAtIndex, deltaSeconds );
		}
	}
}
