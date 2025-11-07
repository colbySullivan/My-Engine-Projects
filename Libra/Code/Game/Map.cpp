#include "Game/Map.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Scorpio.hpp"
#include "Game/Leo.hpp"
#include "Game/Aries.hpp"
#include "Game/Bullet.hpp"



//------------------------------------------------------------------------------
Map::Map( Game* game, IntVec2 dimensions )
	: m_game( game )
	, m_dimensions( dimensions )
{
	m_numTilesInViewVertically = 10;
	m_debugCamera = false;
	BuildMapTiles();
	SpawnNewEntity( ENTITY_TYPE_GOOD_PLAYER, Vec2( 1.5f, 1.5f ), 0.f, FACTION_GOOD );
	SpawnNewEntity( ENTITY_TYPE_EVIL_SCORPIO, Vec2( 5.f, 1.5f ), 0.f, FACTION_EVIL );
	for (int Index = 0; Index < 3 ; ++Index)
	{
		SpawnNewEntity( ENTITY_TYPE_EVIL_LEO, Vec2( 5.f, 5.f ), 0.f, FACTION_EVIL );
	}
	SpawnNewEntity( ENTITY_TYPE_EVIL_ARIES, Vec2( 6.f, 6.f ), 0.f, FACTION_EVIL );
	//SpawnNewEntity( ENTITY_TYPE_GOOD_BULLET, Vec2( 7.f, 7.f ), 0.f, FACTION_EVIL );



	//AddToEntityVector( new Player( m_game, Vec2( 1.5f, 1.5f ) ) );
	//AddToEntityVector( new Scorpio( m_game, Vec2( 5.f, 1.5f ) ) );
	//AddToEntityVector( new Leo( m_game, Vec2( 5.f, 5.f ) ) );
}

//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds)
{
	if ( g_engine->m_input->IsKeyDown( KEYCODE_F4 ) )
	{
		m_debugCamera = !m_debugCamera;
	}
	PushEntityOutOfEachOther();
	UpdateCameras();
	UpdateEntities( deltaSeconds );
	DestroyGarbageEntities();
	m_entityListsByType[ENTITY_TYPE_GOOD_PLAYER];
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
	if ( tileCoords.x < 0 || tileCoords.x >= m_dimensions.x ||
		tileCoords.y < 0 || tileCoords.y >= m_dimensions.y )
	{
		return true;
	}
	return m_tiles[GetTileIndexForTileCoords(tileCoords)].m_type != TILE_TYPE_GRASS;
}

//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfEachOther() const
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size() ; ++entityIndex)
	{
		Entity* firstEntity = m_allEntities[entityIndex];
		if ( firstEntity )
			for ( int otherEntityIndex = entityIndex + 1; otherEntityIndex < m_allEntities.size(); ++otherEntityIndex )
			{
				Entity* otherEntity = m_allEntities[otherEntityIndex];
				if ( otherEntity )
				{
					bool sameFaction = ( firstEntity->m_faction == otherEntity->m_faction );
					bool eitherIsBullet = ( firstEntity->m_entityType == ENTITY_TYPE_GOOD_BULLET ) || ( otherEntity->m_entityType == ENTITY_TYPE_GOOD_BULLET );
					if ( sameFaction && eitherIsBullet )
						continue;

					bool isBPushed = firstEntity->m_doesPushEntities && otherEntity->m_isPushedByEntities;
					bool isAPushed = otherEntity->m_doesPushEntities && firstEntity->m_isPushedByEntities;

					if ( isBPushed && !isAPushed )
					{
						PushDiscOutOfFixedDisc2D( otherEntity->m_position, otherEntity->m_physicsRadius, firstEntity->m_position, firstEntity->m_physicsRadius );
					}
					if ( isAPushed && !isBPushed )
					{
						PushDiscOutOfFixedDisc2D( firstEntity->m_position, firstEntity->m_physicsRadius, otherEntity->m_position, otherEntity->m_physicsRadius );
					}
					else
					{
						PushDiscsOutOfEachOther2D( firstEntity->m_position, firstEntity->m_physicsRadius, otherEntity->m_position, otherEntity->m_physicsRadius );
					}
					
				}
			}
	}
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
	RenderTiles();
	RenderEntities();
}

//-----------------------------------------------------------------------------------------------
void Map::UpdateCameras()
{
	Player* player = nullptr;
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		if ( m_allEntities[i] && m_allEntities[i]->IsPlayer() )
		{
			player = ( Player* )m_allEntities[i];
			break;
		}
	}

	if ( player )
	{
		if ( m_debugCamera )
		{
			m_numTilesInViewVertically = m_dimensions.y;
		}
		else
		{
			m_numTilesInViewVertically = 10;
		}

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
		if ( tile.m_type == TILE_TYPE_GRASS )
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
void Map::RenderEntities() const
{
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		Entity* entity = m_allEntities[i];
		if ( entity == nullptr )
			continue;

		entity->Render();
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
	m_tiles[GetTileIndexForTileCoords(IntVec2(2, 4))].m_type = TILE_TYPE_STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(3, 4))].m_type = TILE_TYPE_STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 4))].m_type = TILE_TYPE_STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 3))].m_type = TILE_TYPE_STONE;
	m_tiles[GetTileIndexForTileCoords(IntVec2(4, 2))].m_type = TILE_TYPE_STONE;
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
				m_tiles[tileIndex].m_type = TILE_TYPE_STONE;
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
				m_tiles[tileIndex].m_type = TILE_TYPE_STONE;
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
			m_tiles[tileIndex].m_type = TILE_TYPE_GRASS;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void Map::AddToEntityVector( Entity* e )
{
	/*m_entities.reserve( static_cast< int >( m_entities.size() ) + 1 );
	m_entities.push_back( e );*/
}

//-----------------------------------------------------------------------------------------------
bool Map::HasLineOfSight( Vec2 posA, Vec2 posB ) const
{
	Vec2 fwdNormal = (posA - posB ).GetNormalized();
	float maxDist = GetDistance2D( posA, posB );
	//RaycastResult2D result = RaycastVsTiles( posA, fwdNormal, maxDist );
	//return !result.m_didImpact;
	return false;
}

//-----------------------------------------------------------------------------------------------
//RaycastResult2D Map::RaycastVsTiles2( Vec2 startPos, Vec2 fwdNormal, float maxDist )
//{
//	constexpr float TINY_STEPS_PER_UNIT = 10.f;
//	int numSteps = static_cast<float>(maxDist * TINY_STEPS_PER_UNIT);
//	Vec2 tinyStepForward = fwdNormal * (maxDist / numSteps);
//	Vec2 currentPos = startPos;
//	for (int i = 0; i < numSteps ; ++i)
//	{
//		currentPos += tinyStepForward;
//		if ( IsPointInSolid( currentPos ) )
//		{
//			RaycastResult2D hitResult;
//			hitResult.m_didImpact = true;
//			hitResult.m_impactPos = currentPos;
//			hitResult.m_impactDist = GetDistance2D( startPos, currentPos );
//			return hitResult;
//		}
//	}
//	RaycastResult2D missResult;
//	hitResult.m_didImpact = false;
//	hitResult.m_impactPos = startPos + (fwdNormal * maxDist);
//	hitResult.m_impactDist = maxDist;
//	return hitResult;
//}

//-----------------------------------------------------------------------------------------------
void Map::UpdateEntities( float deltaSeconds )
{
	for ( int i = 0; i < static_cast< int >( m_allEntities.size() ); i++ )
	{
		Entity* entity = m_allEntities[i];
		if ( entity == nullptr )
			continue;

		entity->Update( deltaSeconds );

		if ( entity->m_isPushedByWalls )
		{
			PushEntityOutOfWalls( *entity, deltaSeconds );
		}
	}
}

//-----------------------------------------------------------------------------------------------
Entity* Map::SpawnNewEntity( EntityType type, Vec2 const& position, float orientationDegrees, EntityFaction faction )
{
	Entity* newEntity = nullptr;

	switch ( type )
	{
	case ENTITY_TYPE_GOOD_PLAYER:
		newEntity = new Player( m_game, position, orientationDegrees, faction );
		break;
	case ENTITY_TYPE_EVIL_LEO:
		newEntity = new Leo( m_game, position, orientationDegrees, faction );
		break;
	case ENTITY_TYPE_EVIL_SCORPIO:
		newEntity = new Scorpio( m_game, position, orientationDegrees, faction );
		break;
	case ENTITY_TYPE_EVIL_ARIES:
		newEntity = new Aries( m_game, position, orientationDegrees, faction );
		break;
	case ENTITY_TYPE_GOOD_BULLET:
		newEntity = new Bullet( m_game, position, orientationDegrees, faction );
		break;
	default:
		return nullptr;
	}

	if ( newEntity )
	{
		AddEntityToMap( *newEntity );
	}

	return newEntity;
}


//-----------------------------------------------------------------------------------------------
void Map::AddEntityToMap( Entity& e )
{
	EntityType type = e.GetEntityType();
	m_entityListsByType[type].push_back( &e );
	m_allEntities.push_back( &e );
}

//-----------------------------------------------------------------------------------------------
void Map::RemoveEntityFromMap( Entity& e )
{
	EntityType type = e.GetEntityType();
	EntityList& listByType = m_entityListsByType[type];
	listByType.erase( std::remove( listByType.begin(), listByType.end(), &e ), listByType.end() );
	m_allEntities.erase( std::remove( m_allEntities.begin(), m_allEntities.end(), &e ), m_allEntities.end() );
}

//-----------------------------------------------------------------------------------------------
void Map::DestroyGarbageEntities()
{
	for (int entityIndex = 0; entityIndex < m_allEntities.size() ; ++entityIndex)
	{
		Entity* grabageEntity = m_allEntities[entityIndex];
		if ( grabageEntity && grabageEntity->m_isGarbage )
		{
			RemoveEntityFromMap(*grabageEntity);
		}
	}
}
